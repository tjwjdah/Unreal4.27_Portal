// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalGunComponent.h"
#include "PortalV2.h"
#include "PortalSurfaceComponent.h"
#include "PortalTravellerComponent.h"

#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

#include "../Character/PP3Character.h"
//#include "PortalPlacementComponent.h"
// Sets default values for this component's properties
UPortalGunComponent::UPortalGunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	NiagaraLaser = CreateDefaultSubobject<UNiagaraSystem>(TEXT("NiagaraLaser"));;
	NiagaraImpact = CreateDefaultSubobject<UNiagaraSystem>(TEXT("NiagaraImpact"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraParticle(TEXT("NiagaraSystem'/Game/MPG/NiagaraElements/FXS_Laser.FXS_Laser'"));
	if (NiagaraParticle.Succeeded())
		NiagaraLaser = NiagaraParticle.Object;
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraParticle1(TEXT("NiagaraSystem'/Game/MPG/NiagaraElements/FXS_LaserImpact.FXS_LaserImpact'"));
	if (NiagaraParticle1.Succeeded())
		NiagaraImpact = NiagaraParticle1.Object;
	DoOnce = true;

	InPortal = nullptr;
	OutPortal = nullptr;
	TraceDistance = 5000.f;
	EdgeMargin = 2.f;
	PortalOffset = 0.1f;
	CheckStartDistance = 5.f;
	CheckDepth = 20.f;
	CheckGap = 2.f;
	MinNormalMatch = 0.98f;
	FrontCheckDepth = 20.f;
	FrontCheckGap = 2.f;
	

	// ...

}


// Called when the game starts
void UPortalGunComponent::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
		PhysicsHandle->bInterpolateTarget = false;
    PlayerTraveller = GetOwner()->FindComponentByClass<UPortalTravellerComponent>();
	APP3Character* Character = Cast<APP3Character>(GetOwner());
	
	if (IsValid(Character)) {
		GrabSocket = Character->GetGrabSocket();
		ArrowComponent = Character->GetArrow();
	}
    if (PlayerTraveller)
        PlayerTraveller->OnPortalTeleport.AddUObject(this, &UPortalGunComponent::OnPlayerTeleport);

	CreatePortal();
	LinkPortal();
	// ...
	
	
}

void UPortalGunComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PlayerTraveller)
		PlayerTraveller->OnPortalTeleport.RemoveAll(this);

	if (GrabTraveller)
		GrabTraveller->OnPortalTeleport.RemoveAll(this);

	GrabRelease();
	if (IsValid(InPortal))
		InPortal->Destroy();

	if (IsValid(OutPortal))
		OutPortal->Destroy();

	Super::EndPlay(EndPlayReason);
}

void UPortalGunComponent::CreatePortal()
{
	if (!InPortalClass || !OutPortalClass)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;
	
	FActorSpawnParameters Params;
	Params.Owner = GetOwner();

	InPortal = World->SpawnActor<APortalV2>(InPortalClass,FVector::ZeroVector,FRotator::ZeroRotator,Params);
	OutPortal = World->SpawnActor<APortalV2>(OutPortalClass,FVector::ZeroVector,FRotator::ZeroRotator,Params);

	if (!IsValid(InPortal) || !IsValid(OutPortal))
	{
		if (IsValid(InPortal)) InPortal->Destroy();
		if (IsValid(OutPortal)) OutPortal->Destroy();

		InPortal = nullptr;
		OutPortal = nullptr;
		return;
	}

	if (InPortal)
		InPortal->DeActivatePortal();
	

	if (OutPortal)
		OutPortal->DeActivatePortal();

	PortalHalfSize = InPortal->GetPortalHalfSize();
}

void UPortalGunComponent::LinkPortal()
{
	if (!InPortal || !OutPortal)
		return;

	InPortal->SetLinkPortal(OutPortal);
	InPortal->HiddenComponentsUpdate();
	OutPortal->SetLinkPortal(InPortal);
	OutPortal->HiddenComponentsUpdate();
}


FRotator UPortalGunComponent::CalculatePortalRotation(const FVector& SurfaceNormal)
{
	FVector Forward = SurfaceNormal.GetSafeNormal();

	FVector ReferenceDirection = FVector::UpVector;
	float UpAlignment = FMath::Abs(FVector::DotProduct(Forward, ReferenceDirection));

	if (UpAlignment > 0.9f)
	{
		AActor* Owner = GetOwner();
		if (Owner)
		{
			UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>();
			if (Camera)
				ReferenceDirection = Camera->GetForwardVector();
		}
	}

	FVector PortalUp = FVector::VectorPlaneProject(ReferenceDirection, Forward).GetSafeNormal();

	if (PortalUp.IsNearlyZero())
		PortalUp = FVector::VectorPlaneProject(FVector::RightVector, Forward).GetSafeNormal();

	return FRotationMatrix::MakeFromXZ(Forward, PortalUp).Rotator();
}

FPortalPlacementResult UPortalGunComponent::FindPortalSpot(APortalV2* TargetPortal,APortalV2* OtherPortal)
{
	FPortalPlacementResult Result;

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return Result;
	}

	UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>();
	if (!Camera)
	{
		
		return Result;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return Result;
	}

	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * TraceDistance;
	LaserEnd = End;
	FHitResult HitResult;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	if (TargetPortal)
		Params.AddIgnoredActor(TargetPortal);

	if (OtherPortal)
		Params.AddIgnoredActor(OtherPortal);


	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECC_Visibility,Params);
	if (!bHit)
	{
		return Result;
	}
	
	AActor* HitActor =HitResult.GetActor();

	UPrimitiveComponent* HitComponent =HitResult.GetComponent();
	
	if (!HitActor || !HitComponent)
	{
		return Result;
	}

	LaserEnd = HitResult.ImpactPoint;
	UPortalSurfaceComponent* PortalSurface = HitResult.GetActor() ? HitResult.GetActor()->FindComponentByClass<UPortalSurfaceComponent>() : nullptr;
	if (!PortalSurface ||!PortalSurface->CanPlacePortal())
	{
		return Result;
	}
	
	TArray<UBoxComponent*> PlaceAreas =PortalSurface->GetPlaceAreas();

	if (PlaceAreas.Num() <= 0)
		return Result;
	UBoxComponent* PlaceArea = nullptr;
	FVector AreaNormal = FVector::ZeroVector;
	FVector CorrectSurfacePoint = FVector::ZeroVector;
	FRotator PortalRotation;

	FVector HitNormal = HitResult.ImpactNormal.GetSafeNormal();

	for (auto* Area : PlaceAreas) {
		if (!IsValid(Area))
			continue;
		FVector CurrentAreaNormal = Area->GetForwardVector().GetSafeNormal();

		if (FVector::DotProduct(CurrentAreaNormal, HitNormal) < MinNormalMatch)
			continue;
		FRotator CurrentPortalRotation = CalculatePortalRotation(CurrentAreaNormal);

		if (!PortalInsidePlaceArea(Area, HitResult.ImpactPoint, CurrentPortalRotation, CorrectSurfacePoint))
			continue;

		PlaceArea = Area;
		AreaNormal = CurrentAreaNormal;
		PortalRotation = CurrentPortalRotation;
		break;
	}
	if(!PlaceArea)
		return Result;

	/*bool bInsidePlaceArea = PortalInsidePlaceArea(PlaceArea, HitResult.ImpactPoint, PortalRotation, CorrectSurfacePoint);

	if (!bInsidePlaceArea)
	{
		return Result;
	}
	*/
	bool bSupported = PortalSupportCheck(CorrectSurfacePoint,PortalRotation,HitComponent,TargetPortal,OtherPortal);

	if (!bSupported)
	{
		return Result;
	}

	bool bBlock = IsPortalBlock(CorrectSurfacePoint,PortalRotation,HitComponent,TargetPortal);

	if (bBlock)
	{
		return Result;
	}
	
	Result.bCanPlace = true;
	
	Result.SurfacePoint =CorrectSurfacePoint;
	LaserEnd = Result.SurfacePoint;
	
	Result.Location =CorrectSurfacePoint+ AreaNormal* PortalOffset;
	
	Result.Rotation =PortalRotation;

	Result.SurfaceComponent =HitComponent;

	Result.PortalSurface =PortalSurface;


	return Result;
}

void UPortalGunComponent::FirePortal(EPortalType PortalType)
{
	APortalV2* TargetPortal =PortalType == EPortalType::PIN? InPortal: OutPortal;

	APortalV2* OtherPortal =PortalType == EPortalType::PIN? OutPortal: InPortal;

	if (!TargetPortal)
		return;


	const FPortalPlacementResult Result =FindPortalSpot(TargetPortal,OtherPortal);
	FireLaser(Result, PortalType);
	if (!Result.bCanPlace)
		return;

	if (bGrabThroughPortal && IsValid(GrabPortal))
	{
		APortalV2* GrabLinkPortal = GrabPortal->GetLinkPortal();

		if (TargetPortal == GrabPortal || TargetPortal == GrabLinkPortal)
			GrabRelease();
	
	}


	TargetPortal->PlaceSurface(Result.Location, Result.Rotation, Result.SurfacePoint, Result.PortalSurface);
	InPortal->AllPortalBlocksBlockOn(true);
	OutPortal->AllPortalBlocksBlockOn(true);
	PortalBoxComponentsCheck(InPortal, Result.SurfaceComponent);
	PortalBoxComponentsCheck(OutPortal, Result.SurfaceComponent);
}

bool UPortalGunComponent::PortalInsidePlaceArea(UBoxComponent* PlaceArea, const FVector& SurfacePoint, const FRotator& PortalRotation, FVector& OutSurfacePoint)
{
	if(!PlaceArea)
	return false;

	FVector AreaCenter = PlaceArea->GetComponentLocation();

	FVector AreaRight = PlaceArea->GetRightVector().GetSafeNormal();
	
	FVector AreaUp = PlaceArea->GetUpVector().GetSafeNormal();

	FVector AreaExtent = PlaceArea->GetScaledBoxExtent();

	FRotationMatrix PortalAxes(PortalRotation);

	FVector PortalRight =PortalAxes.GetUnitAxis(EAxis::Y);

	FVector PortalUp =PortalAxes.GetUnitAxis(EAxis::Z);

	float RequireHalfY = FMath::Abs(FVector::DotProduct(PortalRight, AreaRight)) * PortalHalfSize.X + FMath::Abs(FVector::DotProduct(PortalUp, AreaRight)) * PortalHalfSize.Y + EdgeMargin;

	float RequireHalfZ = FMath::Abs(FVector::DotProduct(PortalRight, AreaUp)) * PortalHalfSize.X + FMath::Abs(FVector::DotProduct(PortalUp, AreaUp)) * PortalHalfSize.Y + EdgeMargin;

	float MaxCenterY =AreaExtent.Y - RequireHalfY;

	float MaxCenterZ =AreaExtent.Z - RequireHalfZ;

	if (MaxCenterY < 0.f ||MaxCenterZ < 0.f)
		return false;
	
	FVector DesiredOffset = SurfacePoint - AreaCenter;

	float DesiredY = FVector::DotProduct(DesiredOffset,AreaRight);

	float DesiredZ = FVector::DotProduct(DesiredOffset,AreaUp);

	float CorrectedY =FMath::Clamp(DesiredY,-MaxCenterY,MaxCenterY);

	float CorrectedZ =FMath::Clamp(DesiredZ,-MaxCenterZ,MaxCenterZ);

	OutSurfacePoint =AreaCenter+ AreaRight * CorrectedY+ AreaUp * CorrectedZ;

	return true;
}

bool UPortalGunComponent::PortalSupportCheck(const FVector& SurfacePoint, const FRotator& PortalRotation, UPrimitiveComponent* Surface, APortalV2* TargetPortal, APortalV2* OtherPortal)
{
	UWorld* World = GetWorld();

	if (!World || !Surface)
		return false;

	FRotationMatrix Axes(PortalRotation);

	FVector Forward =Axes.GetUnitAxis(EAxis::X);

	FVector Right =Axes.GetUnitAxis(EAxis::Y);

	FVector Up = Axes.GetUnitAxis(EAxis::Z);

	float ProbeHalfWidth = PortalHalfSize.X - CheckGap;
	float ProbeHalfHeight =  PortalHalfSize.Y - CheckGap;
	FCollisionQueryParams Params(NAME_None, true);
	Params.bTraceComplex = true;

	AActor* Owner = GetOwner();

	if (Owner)
		Params.AddIgnoredActor(Owner);
	if (TargetPortal)
		Params.AddIgnoredActor(TargetPortal);
	if (OtherPortal)
		Params.AddIgnoredActor(OtherPortal);

	FVector2D ProbePoints[] =
	{
		FVector2D(0.f,  0.f),
		FVector2D(-1.f, -1.f),
		FVector2D(1.f, -1.f),
		FVector2D(1.f,  1.f),
		FVector2D(-1.f,  1.f),
		FVector2D(0.f, -1.f),
		FVector2D(1.f,  0.f),
		FVector2D(0.f,  1.f),
		FVector2D(-1.f,  0.f)
	};

	for (auto& Probe : ProbePoints) {
		FVector ProbePosition = SurfacePoint + Right * Probe.X * ProbeHalfWidth + Up * Probe.Y * ProbeHalfHeight;
		FVector TraceStart = ProbePosition + Forward * CheckStartDistance;
		FVector TraceEnd = ProbePosition - Forward * CheckDepth;

		FHitResult Hit;

		bool bHit = World->LineTraceSingleByChannel(Hit,TraceStart,TraceEnd,ECC_Visibility,Params);
		
		if (!bHit)
			return false;

		if (Hit.GetComponent() != Surface)
			return false;

		float NormalDot =FVector::DotProduct(Hit.ImpactNormal.GetSafeNormal(),Forward);

		if (NormalDot <MinNormalMatch)
			return false;
	}
	return true;
}

bool UPortalGunComponent::IsPortalBlock(const FVector& SurfacePoint, const FRotator& PortalRotation, UPrimitiveComponent* Surface, APortalV2* TargetPortal)
{
	UWorld* World = GetWorld();

	if (!World)
		return true;

	FRotationMatrix Axes(PortalRotation);

	FVector Forward =Axes.GetUnitAxis(EAxis::X);

	FVector BoxCenter = SurfacePoint + Forward * (FrontCheckDepth * 0.5f - PortalOffset);

	FVector BoxHalfExtent(FrontCheckDepth * 0.5f,PortalHalfSize.X- FrontCheckGap,PortalHalfSize.Y- FrontCheckGap);

	FCollisionQueryParams Params(NAME_None, true);
	Params.bTraceComplex = true;
	AActor* Owner = GetOwner();
	if (Owner)
		Params.AddIgnoredActor(Owner);

	if (TargetPortal)
		Params.AddIgnoredActor(TargetPortal);

	if (Surface)
		Params.AddIgnoredComponent(Surface);

	TArray<FOverlapResult> Overlaps;

	bool bBlockingOverlap =World->OverlapMultiByChannel(Overlaps,BoxCenter,PortalRotation.Quaternion(),ECC_Visibility,FCollisionShape::MakeBox(BoxHalfExtent),Params);

	if (!bBlockingOverlap)
		return false;
	
	for (auto& Overlap: Overlaps)
	{
		UPrimitiveComponent* Component =Overlap.GetComponent();

		if (!Component)
			continue;

		if (Component ==Surface)
			continue;

		if (Component->ComponentHasTag(TEXT("PortalBlockBox"))) {
			continue;
		}

		if (Overlap.bBlockingHit)
			return true;
	}

	return false;
}

bool UPortalGunComponent::GetGrabView(FVector& OutLocation, FVector& OutDirection)
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());

	if (!PawnOwner)
		return false;

	AController* Controller = PawnOwner->GetController();

	if (!Controller)
		return false;

	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(OutLocation, ViewRotation);
	OutDirection = ViewRotation.Vector().GetSafeNormal();

	return true;
}

bool UPortalGunComponent::FindGrabHit(FHitResult& OutHit, APortalV2*& OutHitPortal)
{
	OutHit = FHitResult();
	OutHitPortal = nullptr;

	FVector ViewLocation;
	FVector ViewDirection;

	if (!GetGrabView(ViewLocation, ViewDirection))
		return false;

	UWorld* World = GetWorld();
	
	if (!World)return false;

	FVector FirstEnd = ViewLocation + ViewDirection * GrabTraceDistance;

	FCollisionQueryParams Params(NAME_None, true);
	Params.AddIgnoredActor(GetOwner());

	FHitResult Hit;
	bool bHit = World->LineTraceSingleByChannel(Hit, ViewLocation, FirstEnd, ECC_GameTraceChannel12, Params);
	
	if (!bHit)
		return false;
	
	APortalV2* HitPortal = Cast<APortalV2>(Hit.GetActor());
	
	if (!HitPortal || !(HitPortal->GetPortalPlane() == Hit.GetComponent())) {
		OutHit = Hit;
		return true;
	}
	
	APortalV2* LinkPortal = HitPortal->GetLinkPortal();

	if (!HitPortal->GetActive() || !IsValid(LinkPortal) || !LinkPortal->GetActive())
		return false;
	
	float UsedDistance = FVector::Distance(ViewLocation, Hit.ImpactPoint);
	float LeftDistance = GrabTraceDistance - UsedDistance;

	if (LeftDistance <= PortalTraceOffset)
		return false;

	FVector NewDirection = HitPortal->TransformVector(ViewDirection).GetSafeNormal();
	FVector NewStart = HitPortal->TransformPoint(Hit.ImpactPoint) + NewDirection * PortalTraceOffset;
	FVector NewEnd = NewStart + NewDirection * LeftDistance;

	FCollisionQueryParams Params2(NAME_None, true);

	Params2.AddIgnoredActor(GetOwner());
	Params2.AddIgnoredActor(HitPortal);
	Params2.AddIgnoredActor(LinkPortal);

	FHitResult Hit2;

	bool bHit2 = World->LineTraceSingleByChannel(Hit2, NewStart, NewEnd, ECC_GameTraceChannel12, Params2);
	
	if (!bHit2)
		return false;
	OutHit = Hit2;
	OutHitPortal = HitPortal;

	return true;
}

bool UPortalGunComponent::CanGrab(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	UPrimitiveComponent* HitComponent = Hit.GetComponent();

	if (!HitActor || !HitComponent)
		return false;

	return HitComponent->IsSimulatingPhysics();
}

FVector UPortalGunComponent::GetGrabTarget()
{
	if (IsValid(GrabSocket))
		return GrabSocket->GetComponentLocation();

	return FVector::ZeroVector;
}

void UPortalGunComponent::SetGrabPortal(APortalV2* Portal)
{
	if (!IsValid(Portal))
		return;
	GrabPortal = Portal;
	bGrabThroughPortal = true;;
}

void UPortalGunComponent::ClearGrabPortal()
{
	GrabPortal = nullptr;
	bGrabThroughPortal = false;
}

void UPortalGunComponent::ClearGrabData()
{

	GrabbedActor = nullptr;
	GrabbedComponent = nullptr;
	GrabTraveller = nullptr;
	
	ClearGrabPortal();
}

void UPortalGunComponent::OnPlayerTeleport(APortalV2* FromPortal, APortalV2* ToPortal)
{
	if (!IsValid(GrabbedComponent))
		return;

	if (bGrabThroughPortal && GrabPortal == FromPortal)
		ClearGrabPortal();
	else
		SetGrabPortal(ToPortal);

	GrabUpdate();
}

void UPortalGunComponent::OnGrabObjectTeleport(APortalV2* FromPortal, APortalV2* ToPortal)
{

	if (!IsValid(GrabbedComponent))
		return;

	if (bGrabThroughPortal && GrabPortal == ToPortal)
		ClearGrabPortal();
	else
		SetGrabPortal(FromPortal);
	ResetGrabAfterTeleport();
	//GrabUpdate();
}

void UPortalGunComponent::GrabObject()
{
	if (!PhysicsHandle || IsValid(GrabbedComponent))
		return;

	FHitResult GrabHit;
	APortalV2* HitPortal = nullptr;

	if (!FindGrabHit(GrabHit, HitPortal))
		return;

	if (!CanGrab(GrabHit))
		return;

	GrabbedActor = GrabHit.GetActor();
	GrabbedComponent = GrabHit.GetComponent();
	GrabTraveller = GrabbedActor->FindComponentByClass<UPortalTravellerComponent>();

	if (!GrabbedActor || !GrabbedComponent)
	{
		ClearGrabData();
		return;
	}
	
	
	FVector GrabPoint =GrabbedComponent->GetCenterOfMass(NAME_None);
	PhysicsHandle->GrabComponentAtLocationWithRotation(GrabbedComponent,NAME_None, GrabPoint,GrabbedComponent->GetComponentRotation());
	
	if (IsValid(HitPortal))
		SetGrabPortal(HitPortal);
	else
		ClearGrabPortal();

	if (GrabTraveller)
	{
		GrabTraveller->SetGrabObject(true);
		GrabTraveller->OnPortalTeleport.RemoveAll(this);
		GrabTraveller->OnPortalTeleport.AddUObject(this, &UPortalGunComponent::OnGrabObjectTeleport);
	}

	GrabUpdate();
}

void UPortalGunComponent::GrabUpdate()
{

	if (!PhysicsHandle || !IsValid(GrabbedComponent))
			return;

	FVector TargetLocation = GetGrabTarget();

	if (bGrabThroughPortal)
	{
		if (!IsValid(GrabPortal) || !GrabPortal->GetActive() || !IsValid(GrabPortal->GetLinkPortal()) || !GrabPortal->GetLinkPortal()->GetActive())
		{
			GrabRelease();
			return;
		}

		TargetLocation = GrabPortal->TransformPoint(TargetLocation);
	}

	PhysicsHandle->SetTargetLocation(TargetLocation);
}

void UPortalGunComponent::GrabRelease()
{
	if (GrabTraveller)
	{
		GrabTraveller->SetGrabObject(false);
		GrabTraveller->OnPortalTeleport.RemoveAll(this);
	}
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
		PhysicsHandle->ReleaseComponent();

	ClearGrabData();
}

void UPortalGunComponent::GrabLaunch()
{
	if (!IsValid(GrabbedComponent))
		return;

	FVector ViewLocation;
	FVector ThrowDirection;

	if (!GetGrabView(ViewLocation, ThrowDirection))
		return;

	if (bGrabThroughPortal && IsValid(GrabPortal)) {
		ThrowDirection = GrabPortal->TransformVector(ThrowDirection).GetSafeNormal();
	}

	UPrimitiveComponent* ThrowComponent = GrabbedComponent;
	GrabRelease();

	if (IsValid(ThrowComponent) && ThrowComponent->IsSimulatingPhysics())
		ThrowComponent->AddImpulse(ThrowDirection * ThrowPower, NAME_None, true);
}

void UPortalGunComponent::ResetGrabAfterTeleport()
{
	if (!PhysicsHandle || !IsValid(GrabbedComponent))
		return;


	if (PhysicsHandle->GetGrabbedComponent())
		PhysicsHandle->ReleaseComponent();


	GrabbedComponent->WakeAllRigidBodies();
	FVector NewGrabPoint =GrabbedComponent->GetCenterOfMass(NAME_None);

	PhysicsHandle->GrabComponentAtLocationWithRotation(GrabbedComponent,NAME_None,NewGrabPoint,GrabbedComponent->GetComponentRotation());

	GrabUpdate();
}

void UPortalGunComponent::PortalBoxComponentsCheck(APortalV2* Portal, UPrimitiveComponent* Surface)
{
	UWorld* World = GetWorld();

	if (!World)
		return;


	FVector Forward = Portal->GetActorForwardVector();
	FVector BoxCenter = Portal->GetActorLocation() + Forward * (FrontCheckDepth * 0.5f - PortalOffset);
	FVector BoxHalfExtent(FrontCheckDepth * 0.5f, PortalHalfSize.X - FrontCheckGap, PortalHalfSize.Y - FrontCheckGap);
	
	FCollisionQueryParams Params(NAME_None, true);
	Params.bTraceComplex = true;
	AActor* Owner = GetOwner();
	if (Owner)
		Params.AddIgnoredActor(Owner);

	if (IsValid(Portal))
		Params.AddIgnoredActor(Portal);

	if (Surface)
		Params.AddIgnoredComponent(Surface);

	TArray<FOverlapResult> Overlaps;

	bool bBlockingOverlap = World->OverlapMultiByChannel(Overlaps, BoxCenter, Portal->GetActorRotation().Quaternion(), ECC_Visibility, FCollisionShape::MakeBox(BoxHalfExtent), Params);


	if (!bBlockingOverlap)
		return;

	for (auto& Overlap : Overlaps)
	{
		UPrimitiveComponent* Component = Overlap.GetComponent();

		if (!Component)
			continue;

		if (Component->ComponentHasTag(TEXT("PortalBlockBox"))) {
			UBoxComponent* Box = Cast<UBoxComponent>(Component);
			if (Box) {
				Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
	return;
}

bool UPortalGunComponent::PortalSound()
{
	if (!DoOnce)
		return false;

	DoOnce = false;
	if (!IsValid(SoundBase))
		SoundBase = LoadObject<USoundBase>(nullptr, TEXT("SoundCue'/Game/Sounds/SC_Laser.SC_Laser'"));

	if (!IsValid(SoundBase) || !ArrowComponent)
		return false;
	
	LaserSound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundBase, ArrowComponent->GetComponentLocation());
	if (!IsValid(LaserSound) )
		return false;
	LaserSound->AttachToComponent(ArrowComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));

	return true;
}

bool UPortalGunComponent::FireLaser(const FPortalPlacementResult Result, EPortalType PortalType)
{
	Laser = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraLaser, ArrowComponent->GetComponentLocation());
	LaserImpact = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraImpact, ArrowComponent->GetComponentLocation());
	if (!IsValid(Laser) || !IsValid(LaserImpact))
		return false;

	FLinearColor LaserColor = PortalType == EPortalType::PIN ? FVector(0.05f,0.45f,1.f) : FVector(1.f, 0.72f, 0.05f);
	Laser->SetColorParameter(TEXT("Color"), LaserColor);
	LaserImpact->SetColorParameter(TEXT("Color"), LaserColor);


	Laser->SetVectorParameter(TEXT("LaserEnd"), LaserEnd);
	LaserImpact->SetWorldLocation(LaserEnd);
	LaserImpact->SetVisibility(Result.bCanPlace);
	PortalSound();
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, this, &UPortalGunComponent::FireStop, 0.2f, false);

	return true;
}

void UPortalGunComponent::FireStop()
{
	if (IsValid(Laser))
	{
		Laser->DestroyComponent();
		Laser = nullptr;
	}

	if (IsValid(LaserImpact))
	{
		LaserImpact->DestroyComponent();
		LaserImpact = nullptr;
	}

	if (IsValid(LaserSound))
	{
		LaserSound->DestroyComponent();
		LaserSound = nullptr;
	}

	DoOnce = true;
}


// Called every frame
void UPortalGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsValid(GrabbedComponent))
		GrabUpdate();
	// ...
}

