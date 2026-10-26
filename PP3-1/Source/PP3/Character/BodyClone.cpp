// Fill out your copyright notice in the Description page of Project Settings.


#include "BodyClone.h"
#include "PP3Character.h"
#include "../PortalV2/PortalTravellerComponent.h"
#include "../PortalV2/PortalV2.h"
#include "../PortalV2/PortalGunComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
// Sets default values
ABodyClone::ABodyClone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	m_Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	m_ArmsGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsGun"));
	m_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	m_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	m_Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	m_Grabsocket = CreateDefaultSubobject<USceneComponent>(TEXT("Grabsocket"));
	m_Sphere1 = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere1"));
	m_Sphere2 = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere2"));
	SetRootComponent(m_Capsule);
	m_Body->SetupAttachment(m_Capsule);
	m_Gun->SetupAttachment(m_Body);
	m_Camera->SetupAttachment(m_Capsule);
	m_Arms->SetupAttachment(m_Camera);
	m_ArmsGun->SetupAttachment(m_Arms);
	m_Sphere1->SetupAttachment(m_ArmsGun);
	m_Sphere2->SetupAttachment(m_ArmsGun);
	m_Grabsocket->SetupAttachment(m_Sphere1);
	m_PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	
	
}

// Called when the game starts or when spawned
void ABodyClone::BeginPlay()
{
	Super::BeginPlay();
	
	if (!m_Camera)
	return;
	/*
	m_CloneCam = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), m_Camera->GetComponentLocation(), m_Camera->GetComponentRotation());
	if (IsValid(m_CloneCam) && m_CloneCam->GetCameraComponent())
	{
		m_CloneCam->GetCameraComponent()->bConstrainAspectRatio = false;
		FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		m_CloneCam->AttachToComponent(m_Camera, Rule);
		m_CloneCam->SetActorEnableCollision(false);
	}
	*/
	/*
	if (m_Body)
		m_Body->SetVisibility(false, true);
		*/

}

void ABodyClone::SetSource(AActor* Actor)
{
	
	HideClear();
	Super::SetSource(Actor);

	m_Original = Cast<APP3Character>(SourceActor);

	if (!IsValid(m_Original))
		return;
	HiddenPlayerController = Cast<APlayerController>(m_Original->GetController());

	// 플레이어 화면에서 Clone의 1인칭 팔·총을 숨김
	if (APlayerController* PC = HiddenPlayerController.Get())
	{
		PC->HiddenPrimitiveComponents.AddUnique(m_Arms);
		PC->HiddenPrimitiveComponents.AddUnique(m_ArmsGun);
	}
	CloneMaterials.Reset();
	USkeletalMeshComponent* Meshes[] = { m_Body, m_Gun, m_Arms, m_ArmsGun };

	for (USkeletalMeshComponent* Mesh : Meshes)
	{
		if (!IsValid(Mesh)) continue;

		
		Mesh->SetOnlyOwnerSee(false);
		Mesh->SetOwnerNoSee(false);
		Mesh->SetVisibility(true);
		Mesh->SetHiddenInGame(false);

		for (int32 Slot = 0; Slot < Mesh->GetNumMaterials(); ++Slot)
		{
			AddCloneMaterial(Mesh->CreateDynamicMaterialInstance(Slot));
		}
	}

	UpdateAnim();
	
}

void ABodyClone::ShowStart()
{

	OutClone();
}

void ABodyClone::SetCaptureVisibility(USceneCaptureComponent2D* Capture, bool bShowFirstPerson)
{
	if (!IsValid(Capture)) return;

	CaptureCameras.AddUnique(Capture);

	auto SetHidden = [Capture](UPrimitiveComponent* Component, bool bHide)
		{
			if (!IsValid(Component)) return;

			if (bHide)
				Capture->HideComponent(Component);
			else
				Capture->HiddenComponents.Remove(Component);
		};

	SetHidden(m_Arms, !bShowFirstPerson);
	SetHidden(m_ArmsGun, !bShowFirstPerson);

	SetHidden(m_Body, bShowFirstPerson);
	SetHidden(m_Gun, bShowFirstPerson);
}

void ABodyClone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HideClear();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ABodyClone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	if (!IsValid(SourceActor))
		return;

	if (!m_Body || !IsValid(m_Original))
		return;
	
	if (m_Original->GetFollowCamera())
		m_Camera->SetRelativeRotation(m_Original->GetFollowCamera()->GetRelativeRotation());
	*/
}

void ABodyClone::UpdateAnim()
{
	if (!IsValid(SourceActor))
		return;

	
	if (m_ArmsGun && m_Arms)
		m_ArmsGun->AttachToComponent(m_Arms,FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),TEXT("GripPoint"));

	if (m_Body && m_Original->GetCharacterMesh())
		m_Body->SetMasterPoseComponent(m_Original->GetCharacterMesh(), true);

	if (m_Gun && m_Original->GetVisualGunMesh())
		m_Gun->SetMasterPoseComponent(m_Original->GetVisualGunMesh(), true);

	if (m_Arms && m_Original->GetArmsMesh())
		m_Arms->SetMasterPoseComponent(m_Original->GetArmsMesh(), true);

	if (m_ArmsGun && m_Original->GetGunMesh())
		m_ArmsGun->SetMasterPoseComponent(m_Original->GetGunMesh(), true);
	
	//이 함수는 한 스켈레탈 메시 컴포넌트를 다른 스켈레탈 메시 컴포넌트의 "마스터"로 설정합니다.
	//이렇게 하면, "마스터" 컴포넌트의 애니메이션, 변형, 및 다른 변화가 "서브" 컴포넌트에도 적용되게 됩니다.
}

void ABodyClone::UpdateClone(APortalV2* Portal)
{
	if (!IsValid(m_Original) || !IsValid(Portal) || !IsValid(Portal->GetLinkPortal())) return;

	Super::UpdateClone(Portal);

	auto SyncComponent = [Portal](USceneComponent* CloneComponent, USceneComponent* OriginalComponent)
		{
			if (!IsValid(CloneComponent) || !IsValid(OriginalComponent)) return;

			FTransform NewTransform = OriginalComponent->GetComponentTransform();
			NewTransform.SetLocation(Portal->TransformPoint(NewTransform.GetLocation()));
			NewTransform.SetRotation(Portal->TransformRotation(NewTransform.GetRotation()));

			CloneComponent->SetWorldTransform(NewTransform, false, nullptr, ETeleportType::TeleportPhysics);
		};

	// 부모부터 맞춘 뒤 자식 Mesh를 맞춥니다.
	SyncComponent(m_Camera, m_Original->GetFollowCamera());
	SyncComponent(m_Body, m_Original->GetCharacterMesh());
	SyncComponent(m_Gun, m_Original->GetVisualGunMesh());
	SyncComponent(m_Arms, m_Original->GetArmsMesh());
	SyncComponent(m_ArmsGun, m_Original->GetGunMesh());
}

void ABodyClone::HideClear()
{
	OutClone();

	if (APlayerController* PC = HiddenPlayerController.Get())
	{
		PC->HiddenPrimitiveComponents.Remove(m_Arms);
		PC->HiddenPrimitiveComponents.Remove(m_ArmsGun);
	}

	HiddenPlayerController.Reset();
}

void ABodyClone::OutClone()
{

	UPrimitiveComponent* Components[] = { m_Body, m_Gun, m_Arms, m_ArmsGun };

	for (const auto& WeakCapture : CaptureCameras)
	{
		USceneCaptureComponent2D* Capture = WeakCapture.Get();
		if (!IsValid(Capture)) continue;

		for (UPrimitiveComponent* Component : Components)
		{
			if (IsValid(Component))
				Capture->HiddenComponents.Remove(Component);
		}
	}

	CaptureCameras.Reset();
}

