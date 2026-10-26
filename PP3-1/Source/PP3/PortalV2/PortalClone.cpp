// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalClone.h"
#include "PortalV2.h"
// Sets default values
APortalClone::APortalClone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SourceActor = nullptr;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void APortalClone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalClone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortalClone::UpdateClone(APortalV2* Portal)
{
	if (!IsValid(SourceActor) || !IsValid(Portal) || !IsValid(Portal->GetLinkPortal()))
		return;

	FVector NewLocation = Portal->TransformPoint(SourceActor->GetActorLocation());
	FQuat NewRotation = Portal->TransformRotation(SourceActor->GetActorQuat());

	SetActorLocationAndRotation(NewLocation, NewRotation.Rotator(), false, nullptr, ETeleportType::TeleportPhysics);
	SetActorScale3D(SourceActor->GetActorScale3D());
}

void APortalClone::ShowStart()
{
}

void APortalClone::SetClip(APortalV2* Portal, bool bEnable)
{

	APortalV2* ClipPortal = IsValid(Portal) ? Portal->GetLinkPortal() : nullptr;

	for (UMaterialInstanceDynamic* Material : CloneMaterials) {
		if (!Material) 
			continue;

		Material->SetScalarParameterValue(TEXT("ClipOn"), bEnable && IsValid(ClipPortal) ? 1.f : 0.f);

		if (bEnable && IsValid(ClipPortal)) {
			FVector ClipPoint = ClipPortal->GetPortalLocation();
			FVector ClipNormal = ClipPortal->GetActorForwardVector().GetSafeNormal();

			Material->SetVectorParameterValue(TEXT("ClipPoint"), FLinearColor(ClipPoint.X, ClipPoint.Y, ClipPoint.Z,1.f));
			Material->SetVectorParameterValue(TEXT("ClipNormal"), FLinearColor(ClipNormal.X, ClipNormal.Y, ClipNormal.Z, 0.f));
		}
	}
}



void APortalClone::AddCloneMaterial(UMaterialInstanceDynamic* Material)
{
	if (Material)
		CloneMaterials.AddUnique(Material);

}

