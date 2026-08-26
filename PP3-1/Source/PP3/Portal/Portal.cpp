// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "../Character/PP3Character.h"
#include "../Obj/Grab_Obj.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
// Sets default values


APortal::APortal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(m_DefaultSceneRoot);
	m_PortalPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalPlane"));
	m_PortalPlane->SetupAttachment(m_DefaultSceneRoot);

	m_FakeRecursion = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FakeRecursion"));
	m_FakeRecursion->SetupAttachment(m_DefaultSceneRoot);

	m_CylinderTeleport = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderTeleport"));
	m_CylinderTeleport->SetupAttachment(m_DefaultSceneRoot);

	m_CollisionEnabler = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionEnabler"));
	m_CollisionEnabler->SetupAttachment(m_DefaultSceneRoot);

	m_CollisionDisabler = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionDesabler"));
	m_CollisionDisabler->SetupAttachment(m_DefaultSceneRoot);

	m_CollisionCloner = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionCloner"));
	m_CollisionCloner->SetupAttachment(m_DefaultSceneRoot);

	m_Membrane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Membrane"));
	m_Membrane->SetupAttachment(m_DefaultSceneRoot);

	m_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	m_Arrow->SetupAttachment(m_DefaultSceneRoot);


	static ConstructorHelpers::FObjectFinder<UMaterial>	Mtrl(TEXT("Material'/Game/Portal/M_OvalPortal.M_OvalPortal'"));
	if (Mtrl.Succeeded())
		m_PortalMaterialInit = Mtrl.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial>	Mtrl2(TEXT("Material'/Game/Portal/M_OvalFakePortal.M_OvalFakePortal'"));
	if (Mtrl2.Succeeded())
		m_PortalFakeMaterialInit = Mtrl2.Object;

	m_Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	m_Decal->SetupAttachment(m_DefaultSceneRoot);
	m_CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
	m_CaptureComponent->SetupAttachment(m_DefaultSceneRoot);
	m_AnotherPortal = nullptr;
	m_Support = nullptr;
	m_bSharedSupport = false;
	m_Player = nullptr;

	m_SpawnNum = SpawnNum++;

	m_CylinderTeleport->OnComponentBeginOverlap.AddDynamic(this, &APortal::Teleport);
	m_CollisionDisabler->OnComponentBeginOverlap.AddDynamic(this, &APortal::WallPass);
	m_CollisionCloner->OnComponentBeginOverlap.AddDynamic(this, &APortal::Clone);
	m_CollisionEnabler->OnComponentEndOverlap.AddDynamic(this, &APortal::ResetCollision);
	m_CollisionDisabler->OnComponentEndOverlap.AddDynamic(this, &APortal::ResetCollision);
	m_CollisionCloner->OnComponentEndOverlap.AddDynamic(this, &APortal::ResetClone);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	m_PortalRenderTexture = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1920 , 1080, ETextureRenderTargetFormat::RTF_RGBA8);
	if (m_PortalMaterialInit) {
		m_PortalMaterial = m_PortalPlane->CreateDynamicMaterialInstance(0, m_PortalMaterialInit);
		if (m_PortalMaterial)
			m_PortalMaterial->SetTextureParameterValue(TEXT("ConnectedRender"), m_PortalRenderTexture);
	}
	if (m_PortalFakeMaterialInit) {
		m_FakeRecursionMaterial = m_FakeRecursion->CreateDynamicMaterialInstance(0, m_PortalFakeMaterialInit);
		m_FakeRecursion->SetVisibleInSceneCaptureOnly(true);
		if (m_FakeRecursionMaterial)
			m_FakeRecursionMaterial->SetTextureParameterValue(TEXT("ConnectedRender"), m_PortalRenderTexture);
	}
	if (m_PortalMaterial && m_FakeRecursionMaterial)
	{
		if (m_Type == EPortalType::POUT) {
			m_FakeRecursionMaterial->SetVectorParameterValue(TEXT("BorderColor"), FVector(1.0f, 0.0275f, 0.0f));
			m_PortalMaterial->SetVectorParameterValue(TEXT("BorderColor"), FVector(1.0f, 0.0275f, 0.0f));
		}
		else if (m_Type == EPortalType::PIN) 
		{
			m_FakeRecursionMaterial->SetVectorParameterValue(TEXT("BorderColor"), FVector(0.0f, 0.91f, 1.0f));
			m_PortalMaterial->SetVectorParameterValue(TEXT("BorderColor"), FVector(0.0f, 0.91f, 1.0f));
		}
	}
	m_CylinderTeleport->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InitSupportCollisionPreset();

	PortalInit(this);
	/*
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]() {
		PortalInit(m_AnotherPortal);
		}), 0.2f, false);
	*/

	OnDestroyed.AddDynamic(this, &APortal::RemoveClones);
}

void APortal::InitSupportCollisionPreset()
{
	if (!IsValid(m_Support))
		return;
	
	UPrimitiveComponent* SupportPrimitive =Cast<UPrimitiveComponent>(m_Support->GetComponentByClass(UPrimitiveComponent::StaticClass()));
	if (!SupportPrimitive)
		return;
	SharedSupport();
	m_SupportCollisionObjectType = SupportPrimitive->GetCollisionObjectType();
	if (m_bSharedSupport)
	{
		m_NewSupportCollisionType = m_SupportCollisionObjectType;
		m_Channel = m_NewSupportCollisionType;
		InitTagSupport();
		return;
	}
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("SP_Init"), FoundActors);

	TArray<TEnumAsByte<EObjectTypeQuery>> SP = m_SP;
	for (AActor* Act : FoundActors)
	{
		if (!IsValid(Act))
			continue;
		UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Act->GetComponentByClass(UPrimitiveComponent::StaticClass()));

		if (!Primitive)
			continue;

		for (int32 i = 0; i < m_SP.Num(); ++i)
		{
			if (ConvertCollisionPreset(m_SP[i]) == Primitive->GetCollisionObjectType())
			{
				SP.Remove(m_SP[i]);
			}
		}
	}
	m_SP = SP;

	if (m_SP.Num() <= 0)
		return;

	ConvertCollisionPreset(m_SP[0]);
	m_NewSupportCollisionType = m_Channel;
	InitTagSupport();

}

void APortal::SharedSupport()
{
	if (IsValid(m_AnotherPortal)) {
		if (m_AnotherPortal->m_Support == m_Support) {
			m_bSharedSupport = true;
		}
	}
	/*
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APortal::StaticClass(), FoundActors);
	for (auto Actor : FoundActors) {
		APortal* Por = Cast<APortal>(Actor);
		if (Por->m_Support == _Support && this != Actor) {
			m_bSharedSupport = true;
			break;
		}
	}
	*/
}

void APortal::BindOnDestroy()
{

}

void APortal::PortalInit(APortal* _portal)
{
	if (IsValid(_portal)) {
		_portal->m_CylinderTeleport->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		_portal->m_CollisionDisabler->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		_portal->m_CollisionEnabler->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void APortal::InitTagSupport()
{
	if (!IsValid(m_Support))
		return;


	UPrimitiveComponent* SupportPrimitive =
		Cast<UPrimitiveComponent>(m_Support->GetComponentByClass(UPrimitiveComponent::StaticClass()));

	if (!SupportPrimitive)
		return;

	SupportPrimitive->SetCollisionObjectType(m_Channel);

	if (!m_Support->Tags.Contains(TEXT("SP_Init")))
		m_Support->Tags.Add(TEXT("SP_Init"));

}


FVector APortal::TransformVelocityPortal(const FVector& Velocity)
{
	if (!IsValid(m_AnotherPortal))
		return Velocity;
	FVector SrcForward = GetActorQuat().GetForwardVector();
	FVector SrcRight = GetActorQuat().GetRightVector();
	FVector SrcUp = GetActorQuat().GetUpVector();

	FVector DestForward = -m_AnotherPortal->GetActorQuat().GetForwardVector();
	FVector DestRight = -m_AnotherPortal->GetActorQuat().GetForwardVector();
	FVector DestUp = m_AnotherPortal->GetActorQuat().GetUpVector();
	FVector Vel;
	Vel.X = FVector::DotProduct(Velocity, SrcForward);
	Vel.Y = FVector::DotProduct(Velocity, SrcRight);
	Vel.Z = FVector::DotProduct(Velocity, SrcUp);

	return Vel.X * DestForward +
		Vel.Y * DestRight +
		Vel.Z * DestUp;
} 

FVector APortal::TransformLocationPortal(const FVector& Location)
{
	if (!IsValid(m_AnotherPortal))
		return Location;

	FVector Src = GetActorLocation();
	//FVector Src = m_PortalPlane->GetComponentLocation();
	FVector SrcForward = GetActorQuat().GetForwardVector();
	FVector SrcRight = GetActorQuat().GetRightVector();
	FVector SrcUp = GetActorQuat().GetUpVector();

	FVector Dest = m_AnotherPortal->GetActorLocation();
	//FVector Dest = m_AnotherPortal->m_PortalPlane->GetComponentLocation();
	FVector DestForward = -m_AnotherPortal->GetActorQuat().GetForwardVector();
	FVector DestRight = -m_AnotherPortal->GetActorQuat().GetForwardVector();
	FVector DestUp = m_AnotherPortal->GetActorQuat().GetUpVector();

	FVector SrcLocation = Location - Src;

	FVector DestLocation = TransformVelocityPortal(SrcLocation);

	return Dest + DestLocation;
	
}

FQuat APortal::TransformRotationPortal(const FQuat& Rotation)
{
	if (!IsValid(m_AnotherPortal))
		return Rotation;

	FQuat SrcQuat = GetActorQuat();
	FVector DestUp = m_AnotherPortal->GetActorQuat().GetUpVector();
	FQuat DestQuat = m_AnotherPortal->GetActorQuat();

	FQuat Diff = DestQuat * SrcQuat.Inverse();
	FQuat Result = Diff * Rotation;

	// Turn 180 degrees.
	// Multiply the up axis pure quaternion by sin(90) (real part is cos(90))
	FQuat Rotator = FQuat(DestUp, PI);

	return Rotator * Result;
}

FTransform APortal::TransformPortal(const FTransform& Transform)
{
	if (!IsValid(m_AnotherPortal))
		return Transform;

	FVector Location = TransformLocationPortal(Transform.GetLocation());
	FQuat Rotation = TransformRotationPortal(Transform.GetRotation());

	return FTransform(Rotation, Location, Transform.GetScale3D());
}



void APortal::RemoveClones(AActor* _Act)
{
	/*
	if (IsValid(m_Player) && m_Player->m_WalkThroughPortal == this)
	{
		m_Player->m_bCloneAlive = false;
	}

	if (IsValid(m_AnotherPortal))
	{
		m_AnotherPortal->m_AnotherPortal = nullptr;
	}

	m_PortalRenderTexture = nullptr;
	*/
}

ECollisionChannel APortal::ConvertCollisionPreset(EObjectTypeQuery _ObjectTypeQuery)
{
	UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	switch (_ObjectTypeQuery)
	{
	case ObjectTypeQuery8:
		m_Channel = ECollisionChannel::ECC_GameTraceChannel2;
		break;
	case ObjectTypeQuery10:
		m_Channel = ECollisionChannel::ECC_GameTraceChannel4;
		break;
	case ObjectTypeQuery11:
		m_Channel = ECollisionChannel::ECC_GameTraceChannel5;
		break;
	case ObjectTypeQuery12:
		m_Channel = ECollisionChannel::ECC_GameTraceChannel6;
		break;
	case ObjectTypeQuery13:
		m_Channel = ECollisionChannel::ECC_GameTraceChannel7;
		break;
	case ObjectTypeQuery14:
		m_Channel = ECollisionChannel::ECC_GameTraceChannel8;
		break;
	case ObjectTypeQuery15:
		m_Channel = ECollisionChannel::ECC_GameTraceChannel9;
		break;
	case ObjectTypeQuery16:
		m_Channel = ECollisionChannel::ECC_GameTraceChannel10;
		break;
	default:
		break;
	}
	return m_Channel;
}

void APortal::UpdateDeltaRotator()
{
	if (IsValid(m_AnotherPortal)) {

		m_DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(),
			FRotator(m_AnotherPortal->GetActorRotation().Pitch,
				m_AnotherPortal->GetActorRotation().Yaw - 180.f,
				m_AnotherPortal->GetActorRotation().Roll));
	}
}

void APortal::UpdatePortalRender()
{
	/*
	if (!IsValid(m_AnotherPortal) || !m_CaptureComponent)
		return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
		return;

	APP3Character* Player = Cast<APP3Character>(PC->GetPawn());
	if (!Player)
		return;

	UCameraComponent* PlayerCamera = Player->GetFollowCamera();
	if (!PlayerCamera)
		return;

	const FTransform PlayerCameraTransform(
		PlayerCamera->GetComponentQuat(),
		PlayerCamera->GetComponentLocation(),
		FVector::OneVector
	);

	// 중요:
	// CaptureComponent의 TextureTarget이 서로 반대 포탈 텍스처에 연결되어 있으므로
	// this->TransformPortal()이 아니라 m_AnotherPortal->TransformPortal()을 사용한다.
	const FTransform CaptureTransform =
		m_AnotherPortal->TransformPortal(PlayerCameraTransform);

	m_CaptureComponent->SetWorldLocationAndRotation(
		CaptureTransform.GetLocation(),
		CaptureTransform.GetRotation()
	);

	// 이 CaptureComponent는 현재 포탈 쪽에서 렌더링하므로 clip plane은 현재 포탈 면 기준.
	m_CaptureComponent->ClipPlaneBase = GetActorLocation();

	// 화면이 통째로 잘리거나 반대로 보이면 부호를 뒤집어서 테스트.
	m_CaptureComponent->ClipPlaneNormal = -GetActorForwardVector();
	// m_CaptureComponent->ClipPlaneNormal = GetActorForwardVector();
	*/
	if (!IsValid(m_AnotherPortal))
		return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PC || !m_CaptureComponent)
		return;
	const float fmin = 75.f;
	const float fmax = 100.f;

	bool BothTop = GetActorRotation().Pitch > -100.f && GetActorRotation().Pitch < -75.f &&
		m_AnotherPortal->GetActorRotation().Pitch > -100.f && m_AnotherPortal->GetActorRotation().Pitch < -75.f;;
	bool Bothbottom = GetActorRotation().Pitch > fmin && GetActorRotation().Pitch < fmax&&
		m_AnotherPortal->GetActorRotation().Pitch > fmin && m_AnotherPortal->GetActorRotation().Pitch < fmax;
	bool Bothhorizontal = UKismetMathLibrary::Abs(GetActorRotation().Pitch) > fmin && UKismetMathLibrary::Abs(GetActorRotation().Pitch) < fmax &&
		UKismetMathLibrary::Abs(m_AnotherPortal->GetActorRotation().Pitch) > fmin && UKismetMathLibrary::Abs(m_AnotherPortal->GetActorRotation().Pitch) < fmax;

	if (Bothhorizontal) {
		if (BothTop || Bothbottom) {
			//둘다 같은 바닥이나 천장
			m_CaptureComponent->SetWorldRotation(FRotator(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Pitch - 180.f,
				UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Yaw - 180.f,
				UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Roll));
		}
		else {
			//하나는 바닥 하나는 천장
			m_CaptureComponent->SetWorldRotation(FRotator(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation()));
		}
	}
	else {
		if (UKismetMathLibrary::Abs(m_AnotherPortal->GetActorRotation().Pitch) > fmin &&
			UKismetMathLibrary::Abs(m_AnotherPortal->GetActorRotation().Pitch) < fmax) {
			//하나는 바닥 하나는 벽

			if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Vector().Y * m_CaptureComponent->GetForwardVector().Y > 0) {
				m_CaptureComponent->SetWorldRotation(FRotator(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Pitch,
					UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Yaw,
					UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Roll));
			}
			else {
				m_CaptureComponent->SetWorldRotation(FRotator(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Pitch,
					UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Yaw * -1.f,
					UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Roll));
			}

		}
		else {
			//둘다 벽
			m_CaptureComponent->SetWorldRotation(FRotator(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Pitch + m_DeltaRotator.Pitch,
				UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Yaw + m_DeltaRotator.Yaw,
				UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Roll));
		}
	}
	
}

void APortal::Teleport(UPrimitiveComponent* OverlapCom, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	m_SavedVelocity = FVector(0.f, 0.f, 0.f);
	m_FinalVelocity = FVector(0.f, 0.f, 0.f);

	if (!OtherComp || !OtherActor)
		return;

	AActor* OwnerActor = OtherComp->GetOwner();
	APortal* OtherPortal = Cast<APortal>(OtherActor);

	if (OtherPortal && OtherPortal->m_SpawnNum < m_SpawnNum)
	{
		DestroyOtherPortal(OtherPortal);
	
	}

	if (!IsValid(m_AnotherPortal))
		return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);

	if (Character) {
		if (OtherComp->ComponentHasTag(TEXT("Tag_Cam")) && CanTeleport(m_AnotherPortal, Character))
		{
			m_MinimumVelocity = 400.f;
			TeleportPlayer(Character);
		}

		return;
	}
	if (OtherComp->ComponentHasTag(TEXT("Tag_Cube")) && CanTeleport(m_AnotherPortal, OwnerActor))
	{
		AGrab_Obj* GrabObject = Cast<AGrab_Obj>(OwnerActor);

		if (!IsValid(GrabObject))return;

		if (GrabObject->GetGrabbing())
			return;

		m_MinimumVelocity = 400.f;
		TeleportObjects(OwnerActor);
	}

}

void APortal::WallPass(UPrimitiveComponent* OverlapCom, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(m_AnotherPortal) || !OtherComp)
		return;
	UCapsuleComponent* Cap = Cast<UCapsuleComponent>(OtherComp);

	if (Cap)
	{
		ACharacter* Character = Cast<ACharacter>(Cap->GetOwner());

		if (Character && Character->GetCapsuleComponent())
			Character->GetCapsuleComponent()->SetCollisionResponseToChannel(m_NewSupportCollisionType,ECollisionResponse::ECR_Ignore);

		return;
	}
	OtherComp->SetCollisionResponseToChannel(m_NewSupportCollisionType, ECollisionResponse::ECR_Ignore);

}

void APortal::Clone(UPrimitiveComponent* OverlapCom, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	APP3Character* Char = Cast<APP3Character>(OtherComp->GetOwner());
	if (Char) {
		Char->m_WalkThroughPortal = this;
		if (m_LastTeleportedActor != Char) {
			Char->m_bCloneAlive = true;
			Char->CloneManagement();
		}
	}
	else {
		AGrab_Obj* Obj = Cast<AGrab_Obj>(OtherComp->GetOwner());
		if (IsValid(Obj)) {
			Obj->SetWalkThroughtPortal(this);
			if (m_LastTeleportedActor != Obj) {
				Obj->SetCloneAlive(true);
				Obj->CloneManagement();
			}
		}
	}
	*/
}

void APortal::ResetCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(m_AnotherPortal) || !OtherComp)
		return;
	UCapsuleComponent* Cap = Cast<UCapsuleComponent>(OtherComp);
	
	if (Cap) {
		ACharacter* C = Cast<ACharacter>(Cap->GetOwner());
		if (C) {
			C->GetCapsuleComponent()->SetCollisionResponseToChannel(m_NewSupportCollisionType, ECollisionResponse::ECR_Block);
			C->GetCapsuleComponent()->SetCollisionResponseToChannel(m_AnotherPortal->m_NewSupportCollisionType, ECollisionResponse::ECR_Block);
		}
	}
	else 
	{
		OtherComp->SetCollisionResponseToChannel(m_NewSupportCollisionType, ECollisionResponse::ECR_Block);
		OtherComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		OtherComp->SetCollisionResponseToChannel(m_AnotherPortal->m_NewSupportCollisionType, ECollisionResponse::ECR_Block);
		
	}

}

void APortal::TeleportPlayer(ACharacter* _Player)
{
	
	if (!IsValid(_Player) || !IsValid(m_AnotherPortal)) return;
	
	FVector BeforeLocation = _Player->GetActorLocation();
	FVector BeforeVelocity = _Player->GetVelocity();

	FVector AfterVelocity =
		TransformVelocityPortal(BeforeVelocity);
	FVector AfterLocation =
		TransformLocationPortal(BeforeLocation);

	FQuat BeforeQuat = _Player->GetActorQuat();
	FQuat AfterQuat =
		TransformRotationPortal(BeforeQuat);

	_Player->SetActorLocation(AfterLocation, false, nullptr, ETeleportType::None);

	if (APP3Character* Player =
		Cast<APP3Character>(_Player))
	{
		
		auto* Controller = Player->GetController();
		FQuat BefreControllerQuat =
			Controller->GetControlRotation().Quaternion();
		FQuat AfterControllerQUat =
			TransformRotationPortal(BefreControllerQuat);

		Player->GetMovementComponent()->Velocity = AfterVelocity;
		Player->GetController()->SetControlRotation(AfterControllerQUat.Rotator());

		// character always on the ground straightly, so
		// rotate character actor by only axis z(yaw)
		FRotator PlayerActorRotator(
			0.0, AfterQuat.Rotator().Yaw, 0.0);

		Player->SetActorRotation(PlayerActorRotator);

		return;
	}

	_Player->SetActorRotation(AfterQuat);

	_Player->GetRootComponent()->ComponentVelocity = AfterVelocity;
	UPrimitiveComponent* PrimitiveComponent =
		Cast<UPrimitiveComponent>(_Player->GetComponentByClass(UPrimitiveComponent::StaticClass()));
	PrimitiveComponent->SetAllPhysicsLinearVelocity(AfterVelocity, false);
	PlayerCloneDestroy(_Player);
	UpdateTeleportationAuthorization();
}

void APortal::TeleportObjects(AActor* _Actor)
{
	
	if (!_Actor || !m_AnotherPortal) return;
	/*
	UPrimitiveComponent* PrimitiveComponent =Cast<UPrimitiveComponent>(_Actor->GetRootComponent());

	if (!PrimitiveComponent)
		PrimitiveComponent = Cast<UPrimitiveComponent>(_Actor->GetComponentByClass(UPrimitiveComponent::StaticClass()));

	if (!PrimitiveComponent)
		return;

	m_LastTeleportedActor = _Actor;
	m_AnotherPortal->m_LastTeleportedActor = _Actor;

	FVector OldLocation = _Actor->GetActorLocation();
	FQuat OldRotation = _Actor->GetActorQuat();

	FVector OldLinearVelocity = PrimitiveComponent->GetPhysicsLinearVelocity();
	FVector OldAngularVelocity = PrimitiveComponent->GetPhysicsAngularVelocityInRadians();

	if (OldLinearVelocity.IsNearlyZero())
		OldLinearVelocity = _Actor->GetVelocity();

	//FVector NewLocation = TransformPositionPortal(OldLocation);
	FQuat NewRotation = TransformRotationPortal(OldRotation);

	float ExitOffset = 10.f;
	//NewLocation += m_AnotherPortal->GetActorForwardVector() * ExitOffset;

	FVector NewLinearVelocity = CalculateExitVelocity(OldLinearVelocity,1500.0f,m_MinimumVelocity);

	//FVector NewAngularVelocity = TransformDirectionPortal(OldAngularVelocity);
	_Actor->SetActorLocationAndRotation(NewLocation, NewRotation.Rotator(), false, nullptr, ETeleportType::TeleportPhysics);

	PrimitiveComponent->SetPhysicsLinearVelocity(NewLinearVelocity, false);
	PrimitiveComponent->SetPhysicsAngularVelocityInRadians(NewAngularVelocity, false);

	UpdateTeleportationAuthorization();

	/*
	m_SavedVelocity = UKismetMathLibrary::Vector_ClampSizeMax(_Actor->GetVelocity(), 1500.f);
	m_AnotherPortal->m_LastTeleportedActor = _Actor;
	_Actor->SetActorLocation(m_AnotherPortal->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	UStaticMeshComponent* PriCom = Cast<UStaticMeshComponent>(_Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (!PriCom)return;
	
	GetFinalVelocity();
	PriCom->SetAllPhysicsLinearVelocity(m_FinalVelocity);
	UpdateTeleportationAuthorization();
	*/
}

bool APortal::CanTeleport(APortal* _OtherPortal, AActor* _Actor)
{
	if (!IsValid(_OtherPortal) || !IsValid(_Actor))
	{
		return false;
	}

	return (_OtherPortal->m_LastTeleportedActor != _Actor) &&
		(m_LastTeleportedActor != _Actor);
}

void APortal::DestroyOtherPortal(APortal* _OtherPortal)
{
	if (IsValid(_OtherPortal->m_AnotherPortal)) {
		_OtherPortal->m_AnotherPortal->m_CaptureComponent->TextureTarget = nullptr;
		_OtherPortal->m_AnotherPortal = nullptr;

		_OtherPortal->DestroyPortal();
	}
	else {
		_OtherPortal->DestroyPortal();
	}
}

void APortal::DestroyPortal()
{
	m_bSharedSupport = false;
	if (IsValid(m_AnotherPortal))
		m_AnotherPortal->m_AnotherPortal = nullptr;

	if (IsValid(m_Support)) {

		SharedSupport();

		if (!m_bSharedSupport) {
			Cast<UPrimitiveComponent>(m_Support->GetComponentByClass(UPrimitiveComponent::StaticClass()))->SetCollisionObjectType(m_SupportCollisionObjectType);
			TArray<FName> TagCopy = m_Support->Tags;
			for (auto Tag : m_Support->Tags) {
				if (UKismetStringLibrary::Contains(Tag.ToString(), TEXT("SP"))) {
					TagCopy.Remove(Tag);
				}
			}
			m_Support->Tags = TagCopy;
		}
		Destroy();
	}
	else
		if (IsValid(this))
			Destroy();

}

void APortal::UpdatePlayerVelocity(ACharacter* _Player)
{
	GetFinalVelocity();
	if (m_FinalVelocity.Z > 600.f && m_FinalVelocity.Z < 900.f) {

		m_FinalVelocity.Z = 600.f;
	}

	_Player->GetCharacterMovement()->Velocity = m_FinalVelocity;
}

void APortal::UpdatePlayerRotation(ACharacter* _Player)
{
	if (IsValid(_Player)) {
		if (IsValid(_Player->GetController())) {

			if (m_DefaultSceneRoot->GetComponentRotation().Pitch > 75.f && m_DefaultSceneRoot->GetComponentRotation().Pitch < 100.f) {
				if (m_AnotherPortal->RootComponent->GetComponentRotation().Pitch > 75.f && m_AnotherPortal->RootComponent->GetComponentRotation().Pitch < 100.f) {
					//2 horizontal portals on the floor
					m_FinalPlayerRot = FRotator(_Player->GetControlRotation().Pitch, _Player->GetControlRotation().Yaw - m_DeltaRotator.Yaw, _Player->GetControlRotation().Roll);
				}
				else {
					if (UKismetMathLibrary::Abs(m_AnotherPortal->RootComponent->GetComponentRotation().Pitch) > 75.f &&
						UKismetMathLibrary::Abs(m_AnotherPortal->RootComponent->GetComponentRotation().Pitch) < 100.f) {
						//2 horizontal portals :1 on the floor 1 on the roof
						m_FinalPlayerRot = _Player->GetControlRotation();
					}
					else {
						//1 portal on the roof 1 on the wall
						m_FinalPlayerRot = FRotator(_Player->GetControlRotation().Pitch,
							m_AnotherPortal->RootComponent->GetComponentRotation().Yaw, _Player->GetControlRotation().Roll);
					}
				}
			}
			else {
				//2 portals on walls
				m_FinalPlayerRot = FRotator(_Player->GetControlRotation().Pitch - m_DeltaRotator.Pitch,
					_Player->GetControlRotation().Yaw - m_DeltaRotator.Yaw, _Player->GetControlRotation().Roll);
			}
			_Player->GetController()->SetControlRotation(m_FinalPlayerRot);
		}
	}
}

void APortal::PlayerCloneDestroy(ACharacter* _Player)
{
	APP3Character* Player = Cast<APP3Character>(_Player);
	//if (Player)
		//Player->m_bCloneAlive = false;
}

void APortal::UpdateTeleportationAuthorization()
{
	if (!IsValid(m_AnotherPortal))
		return;

	FTimerHandle WaitHandle;
	
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]() {
			if (IsValid(m_AnotherPortal)) {
				m_AnotherPortal->m_LastTeleportedActor = nullptr;
			}
			if (IsValid(this)) {
			m_LastTeleportedActor = nullptr;
			}
	}), 0.2f, false);
	
}

void APortal::GetFinalVelocity()
{
	if (m_SavedVelocity.Size() > m_MinimumVelocity) {
		FVector FDot;
		m_FinalVelocity = UKismetMathLibrary::Dot_VectorVector(m_SavedVelocity, GetActorForwardVector()) * m_AnotherPortal->GetActorForwardVector() +
			UKismetMathLibrary::Dot_VectorVector(m_SavedVelocity, GetActorRightVector()) * m_AnotherPortal->GetActorRightVector() +
			UKismetMathLibrary::Dot_VectorVector(m_SavedVelocity, GetActorUpVector()) * m_AnotherPortal->GetActorUpVector();

		m_FinalVelocity *= -1.f;
	}
	else {
		FVector AnotherVector = m_AnotherPortal->GetActorForwardVector();
		AnotherVector.Normalize();
		m_FinalVelocity = AnotherVector *
			UKismetMathLibrary::SelectFloat(m_SavedVelocity.Size(), m_MinimumVelocity, m_SavedVelocity.Size() > m_MinimumVelocity);;
	}

}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
	/*
	DrawDebugDirectionalArrow(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 100.f,
		20.f,
		FColor::Red,
		false,
		0.f,
		0,
		3.f
	);

	DrawDebugDirectionalArrow(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + GetActorRightVector() * 100.f,
		20.f,
		FColor::Green,
		false,
		0.f,
		0,
		3.f
	);

	DrawDebugDirectionalArrow(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + GetActorUpVector() * 100.f,
		20.f,
		FColor::Blue,
		false,
		0.f,
		0,
		3.f
	);
	*/
	UpdateDeltaRotator();
	UpdatePortalRender();
	
	if (IsValid(m_AnotherPortal)) {
		m_AnotherPortal->m_CaptureComponent->ClipPlaneBase = m_AnotherPortal->m_Decal->GetComponentLocation() + (m_AnotherPortal->m_Decal->GetForwardVector() * -100.f);
		m_AnotherPortal->m_CaptureComponent->ClipPlaneNormal = m_AnotherPortal->m_CaptureComponent->GetForwardVector();

	}
	

	/*
	if (m_Player) {
		//m_CaptureComponent->SetWorldLocation(m_Player->GetActorLocation());
		FVector VAngle = m_Player->GetActorLocation() - GetActorLocation();
		VAngle.Normalize();
		float FDistance = FVector::Distance(m_Player->GetActorLocation(), GetActorLocation());
		if (FDistance >= 1000)
			FDistance = 1000.f;
		FDistance = FDistance / 2000.f;
		if (FDistance < 0.2)
			FDistance = 0.f;
		FDistance = 1.f - FDistance;

	}
	*/
}


void APortal::ResetClone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	APP3Character* Char = Cast<APP3Character>(OtherComp->GetOwner());
	if (Char) {
		/*
		Char->m_WalkThroughPortal = this;
		if (m_LastTeleportedActor != Char) {
			Char->m_bCloneAlive = true;
			Char->CloneManagement();
		}
		*/
	}
	else {
		AGrab_Obj* Obj = Cast<AGrab_Obj>(OtherComp->GetOwner());
		if (IsValid(Obj)) {
			Obj->SetWalkThroughtPortal(nullptr);
			m_LastTeleportedActor = nullptr;
			Obj->SetCloneAlive(false);
			Obj->CloneManagement();
		}
	}
}
