// Fill out your copyright notice in the Description page of Project Settings.


#include "BodyClone.h"
#include "PP3Character.h"
#include "../PortalV2/PortalTravellerComponent.h"
#include "../PortalV2/PortalV2.h"
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
	

	Super::SetSource(Actor);

	m_Original = Cast<APP3Character>(SourceActor);

	if (!IsValid(m_Original))
		return;
	TSet<APortalV2*> Portals;
	if (SourceActor)
	{
		UPortalTravellerComponent* Traveller = SourceActor->FindComponentByClass<UPortalTravellerComponent>();
		if (Traveller)
			Portals = Traveller->GetInPortals();
	}
	for (auto& portal : Portals) {
		portal->GetPortalCamera()->HideComponent(m_Body);
		portal->GetPortalCamera()->HideComponent(m_Gun);
		portal->GetLinkPortal()->GetPortalCamera()->HideComponent(m_Body);
		portal->GetLinkPortal()->GetPortalCamera()->HideComponent(m_Gun);
		//portal->GetLinkPortal()->GetPortalCamera()->HideComponent(m_Arms);
		//portal->GetLinkPortal()->GetPortalCamera()->HideComponent(m_ArmsGun);
		
	}

	UMaterialInstanceDynamic* Material = m_Body->CreateDynamicMaterialInstance(0);
	if (Material)
		AddCloneMaterial(Material);
	Material = m_Gun->CreateDynamicMaterialInstance(0);
	if (Material)
		AddCloneMaterial(Material);
	UpdateAnim();
	
}

// Called every frame
void ABodyClone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsValid(SourceActor))
		return;

	if (!m_Body || !IsValid(m_Original))
		return;
	
	if (m_Original->GetFollowCamera())
		m_Camera->SetRelativeRotation(m_Original->GetFollowCamera()->GetRelativeRotation());
	
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
	Super::UpdateClone(Portal);
	
}

void ABodyClone::HideClear()
{
	TSet<APortalV2*> Portals;
	if (SourceActor)
	{
		UPortalTravellerComponent* Traveller = SourceActor->FindComponentByClass<UPortalTravellerComponent>();
		if (Traveller)
			Portals = Traveller->GetInPortals();
	}
	for (auto& portal : Portals) {
		portal->GetPortalCamera()->HiddenComponents.Remove(m_Body);
		portal->GetPortalCamera()->HiddenComponents.Remove(m_Gun);
		portal->GetLinkPortal()->GetPortalCamera()->HiddenComponents.Remove(m_Body);
		portal->GetLinkPortal()->GetPortalCamera()->HiddenComponents.Remove(m_Gun);
		//portal->GetLinkPortal()->GetPortalCamera()->HiddenComponents.Remove(m_Arms);
		//portal->GetLinkPortal()->GetPortalCamera()->HiddenComponents.Remove(m_ArmsGun);
		
	}
}

