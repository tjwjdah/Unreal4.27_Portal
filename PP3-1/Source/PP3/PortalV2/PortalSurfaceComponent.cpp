// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalSurfaceComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInstanceDynamic.h"


// Sets default values for this component's properties
UPortalSurfaceComponent::UPortalSurfaceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SurfacePrimitive = nullptr;

	SurfaceTag = TEXT("PortalSurface");
	PlaceAreaTag = TEXT("PortalPlaceArea");
	SurfaceDepth = 20.f;
	bCanPlace = true;

	// ...
}


// Called when the game starts
void UPortalSurfaceComponent::BeginPlay()
{
	Super::BeginPlay();
	PortalHoles.SetNum(2);

	FindSurfacePrimitive();
	FindPlaceArea();
	CreateSurfaceMaterialInstances();

	ClearPortalHole(EPortalType::PIN);
	ClearPortalHole(EPortalType::POUT);
	// ...
	
}




// Called every frame
void UPortalSurfaceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int32 UPortalSurfaceComponent::GetPortalIndex(EPortalType PortalType)
{
	return PortalType == EPortalType::PIN? 0 : 1;
}

void UPortalSurfaceComponent::FindSurfacePrimitive()
{
	AActor* Owner = GetOwner();

	if (!Owner)return;

	TArray<UPrimitiveComponent*> Primitives;

	Owner->GetComponents<UPrimitiveComponent>(Primitives);

	for (UPrimitiveComponent* Primitive : Primitives) {
		if (!Primitive)continue;

		if (Primitive->ComponentHasTag(SurfaceTag)){
			SurfacePrimitive=Primitive;
			return;
		}
	}
}

void UPortalSurfaceComponent::CreateSurfaceMaterialInstances()
{
	if (!SurfacePrimitive) 
		return;

	SurfaceMaterialInstances.Empty();

	int32 Count = SurfacePrimitive->GetNumMaterials();
	for (int32 i = 0; i < Count; i++)
	{
		UMaterialInstanceDynamic* MID = SurfacePrimitive->CreateDynamicMaterialInstance(i);

		if (MID)
			SurfaceMaterialInstances.Add(MID);
	}
}

void UPortalSurfaceComponent::UpdatePortalMaterial(int32 SlotIndex)
{
    if (!PortalHoles.IsValidIndex(SlotIndex))
        return;
	 FPortalHoleData& Data =PortalHoles[SlotIndex];

	 FName EnabledName(*FString::Printf(TEXT("PortalEnabled%d"), SlotIndex));

	 FName CenterName(*FString::Printf(TEXT("PortalCenter%d"), SlotIndex));

	 FName ForwardName(*FString::Printf(TEXT("PortalForward%d"), SlotIndex));

	 FName RightName(*FString::Printf(TEXT("PortalRight%d"), SlotIndex));

	 FName UpName(*FString::Printf(TEXT("PortalUp%d"), SlotIndex));

	 FName ExtentsName(*FString::Printf(TEXT("PortalExtents%d"), SlotIndex));


    FLinearColor CenterValue(Data.Center.X,Data.Center.Y,Data.Center.Z,1.f);

    FLinearColor ForwardValue(Data.Forward.X,Data.Forward.Y,Data.Forward.Z,0.f);

    FLinearColor RightValue(Data.Right.X,Data.Right.Y,Data.Right.Z,0.f);

    FLinearColor UpValue(Data.Up.X,Data.Up.Y,Data.Up.Z,0.f);

    FLinearColor ExtentsValue(Data.HalfWidth,Data.HalfHeight,Data.HalfDepth,0.f);

    for (UMaterialInstanceDynamic* MID : SurfaceMaterialInstances)
    {
        if (!MID)
            continue;

        MID->SetScalarParameterValue(EnabledName,Data.bEnabled ? 1.f : 0.f);

        MID->SetVectorParameterValue(CenterName,CenterValue);

        MID->SetVectorParameterValue(ForwardName,ForwardValue);

        MID->SetVectorParameterValue(RightName,RightValue);

        MID->SetVectorParameterValue(UpName,UpValue);

        MID->SetVectorParameterValue(ExtentsName,ExtentsValue);
    }
}

void UPortalSurfaceComponent::SetPortalHole(EPortalType PortalType, const FPortalHoleData& HoleData)
{
	int32 SlotIndex = GetPortalIndex(PortalType);

	if (!PortalHoles.IsValidIndex(SlotIndex))
		return;

	PortalHoles[SlotIndex] = HoleData;
	PortalHoles[SlotIndex].bEnabled = true;
	UpdatePortalMaterial(SlotIndex);
}

void UPortalSurfaceComponent::ClearPortalHole(EPortalType PortalType)
{
	int32 SlotIndex = GetPortalIndex(PortalType);

	if (!PortalHoles.IsValidIndex(SlotIndex))
		return;

	PortalHoles[SlotIndex].bEnabled = false;

	UpdatePortalMaterial(SlotIndex);
}

void UPortalSurfaceComponent::FindPlaceArea()
{
	PlaceAreas.Empty();
	AActor* Owner = GetOwner();

	if (!Owner)
		return;


	TArray<UBoxComponent*> BoxComponents;
	Owner->GetComponents<UBoxComponent>(BoxComponents);

	for (UBoxComponent* Box : BoxComponents)
	{
		if (!Box)
			continue;


		if (Box->ComponentHasTag(PlaceAreaTag))
		{
			PlaceAreas.Add(Box);
		}
	}
}

ECollisionChannel UPortalSurfaceComponent::GetSurfaceCollisionChannel()
{
	if (!IsValid(SurfacePrimitive))
		return ECC_MAX;

	return SurfacePrimitive->GetCollisionObjectType();
}

