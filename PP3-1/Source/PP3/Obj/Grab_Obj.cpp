// Fill out your copyright notice in the Description page of Project Settings.


#include "Grab_Obj.h"
#include "../Portal/Portal.h"
#include "../PortalV2/PortalTravellerComponent.h"
// Sets default values
AGrab_Obj::AGrab_Obj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	
	m_StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(m_StaticMesh);
	PortalTraveller = CreateDefaultSubobject<UPortalTravellerComponent>(TEXT("PortalTraveller"));
	m_Grabbing = false;
	m_bIClone = false;
	m_bPortalInGrab = false;

}

// Called when the game starts or when spawned
void AGrab_Obj::BeginPlay()
{
	Super::BeginPlay();
	if (m_StaticMesh && m_Material)
	{
		m_MaterialInstance = m_StaticMesh->CreateDynamicMaterialInstance(0, m_Material, TEXT("None"));

		if (m_MaterialInstance)
		{
			m_MaterialInstance->SetVectorParameterValue(TEXT("Color"), m_LaserColor);
			m_StaticMesh->SetMaterial(0, m_MaterialInstance);
		}
	}

}

// Called every frame
void AGrab_Obj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_bIClone && m_bPortalInGrab)
		return;
	if (m_bCloneAlive) {
		if (IsValid(m_WalkThroughPortal)) {
			if (IsValid(m_Clone)) {
				FTransform Trans = UKismetMathLibrary::ConvertTransformToRelative(GetActorTransform(), m_WalkThroughPortal->GetActorTransform());
				FVector ConvertVector = GetActorLocation()- m_WalkThroughPortal->GetActorLocation();
				ConvertVector.X *= -1.f;
				
				
				FRotator ConvertRotator = FRotator(GetActorRotation().Pitch - m_WalkThroughPortal->GetDeltaRotator().Pitch, 
					GetActorRotation().Yaw - m_WalkThroughPortal->GetDeltaRotator().Yaw,
					GetActorRotation().Roll - m_WalkThroughPortal->GetDeltaRotator().Roll);
				
				m_Clone->SetActorRotation(ConvertRotator);
				m_Clone->SetActorRelativeLocation(ConvertVector);
				

			}

			if (IsValid(m_WalkThroughPortal->GetAnotherPortal())) {
				m_bInsidePortal = false;
				TArray<AActor*> FoundActors;
				GetOverlappingActors(FoundActors, APortal::StaticClass());
				if (FoundActors.Num() > 0) {
					APortal* InPortal = Cast<APortal>(FoundActors[0]);
					if (IsValid(InPortal)) {
						TArray<UPrimitiveComponent*> FoundComponent;
						GetOverlappingComponents(FoundComponent);
						for (auto Comp : FoundComponent) {
							if (Comp == InPortal->GetCollisionCloner()) {
								m_bInsidePortal = true;
								break;
							}
						}
						if (!m_bInsidePortal && !m_Grabbing) {
							
							m_bCloneAlive = false;
						}
					}
				}
			}
			else {
				m_bCloneAlive = false;
				CloneManagement();
			}
		}
		else {
			m_bCloneAlive = false;
			CloneManagement();
		}
	}
	else {

		CloneManagement();
	}
}


void AGrab_Obj::SetForceDirection(FVector _Dir, float _Force)
{
	m_StaticMesh->AddImpulse(_Dir * _Force, TEXT("None"), true);
}

void AGrab_Obj::CloneManagement()
{
	if (m_bIClone)
		return;
	if (IsValid(m_Clone)) {
		if (!m_bCloneAlive) {
			
			//clone destroy
			m_Clone->SetActorHiddenInGame(false);
			m_WalkThroughPortal = nullptr;
			m_Clone->Destroy();
			m_Clone = nullptr;
			
		}
	}
	else
	{
		if (m_bCloneAlive) {
			if (IsValid(m_WalkThroughPortal) && IsValid(m_WalkThroughPortal->GetAnotherPortal()) && m_CloneBp) {
				//m_BodyClone ½ºÆù

				m_Clone = GetWorld()->SpawnActor<AGrab_Obj>(m_CloneBp);
				m_Clone->m_bIClone = true;
				m_Clone->m_LaserColor = m_LaserColor;
				m_Clone->MaterialSet();
				m_Clone->m_StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				m_Clone->m_StaticMesh->SetSimulatePhysics(false);
				FAttachmentTransformRules Rule(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
				//m_Clone->AttachToActor(m_WalkThroughPortal->GetAnotherPortal(), Rule);
				m_ClonePitchInit = GetActorRotation().Pitch - m_WalkThroughPortal->GetDeltaRotator().Pitch;

				FTransform Trans = UKismetMathLibrary::ConvertTransformToRelative(GetActorTransform(), m_WalkThroughPortal->GetActorTransform());
				
				FVector ConvertVector = Trans.GetLocation() *-1.F ;
				
				FRotator ConvertRotator = FRotator(m_ClonePitchInit, GetActorRotation().Yaw - m_WalkThroughPortal->GetDeltaRotator().Yaw,
					GetActorRotation().Roll);
				
				m_Clone->SetActorRotation(ConvertRotator);
				m_Clone->SetActorRelativeLocation(ConvertVector);
			}

		}
	}
}

void AGrab_Obj::MaterialSet()
{
	if (m_MaterialInstance)
		m_MaterialInstance->SetVectorParameterValue(TEXT("Color"), m_LaserColor);
}

void AGrab_Obj::CloneDestroy()
{
	if (IsValid(m_Clone))
	{
		m_Clone->SetActorHiddenInGame(false);
		m_Clone->Destroy();
		m_Clone = nullptr;
	}

	m_WalkThroughPortal = nullptr;
}

