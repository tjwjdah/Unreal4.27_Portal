// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "Components/ActorComponent.h"
#include "PortalSurfaceComponent.generated.h"

class UPrimitiveComponent;
class UMaterialInstanceDynamic;
class UBoxComponent;
USTRUCT(BlueprintType)
struct FPortalHoleData
{
    GENERATED_BODY()

    UPROPERTY()
    FVector Center = FVector::ZeroVector;

    UPROPERTY()
    FVector Forward = FVector::ForwardVector;

    UPROPERTY()
    FVector Right = FVector::RightVector;

    UPROPERTY()
    FVector Up = FVector::UpVector;

    UPROPERTY()
    float HalfWidth = 91.f;

    UPROPERTY()
    float HalfHeight = 131.f;

    UPROPERTY()
    float HalfDepth = 12.f;

    UPROPERTY()
    bool bEnabled = false;
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PP3_API UPortalSurfaceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPortalSurfaceComponent();

private:
    UPROPERTY(EditAnywhere)
    bool bCanPlace;

    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.1"))
    float SurfaceDepth;

    UPROPERTY(EditAnywhere)
    FName SurfaceTag;

    UPROPERTY(EditAnywhere)
    FName PlaceAreaTag;

    UPROPERTY(VisibleInstanceOnly, Transient)
    UPrimitiveComponent* SurfacePrimitive;

    UPROPERTY(VisibleInstanceOnly, Transient)
    TArray<UBoxComponent*> PlaceAreas;

    UPROPERTY(Transient)
    TArray<UMaterialInstanceDynamic*> SurfaceMaterialInstances;

    TArray<FPortalHoleData> PortalHoles;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
    
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    int32 GetPortalIndex(EPortalType PortalType);

    void FindSurfacePrimitive();
  
    void CreateSurfaceMaterialInstances();

    void UpdatePortalMaterial(int32 SlotIndex);
    bool CanPlacePortal(){ return bCanPlace; }

    float GetSurfaceDepth(){ return SurfaceDepth; }

    void SetPortalHole(EPortalType PortalType,const FPortalHoleData& HoleData);

    void ClearPortalHole(EPortalType PortalType);
    TArray<UBoxComponent*>& GetPlaceAreas() { return PlaceAreas; }
    void FindPlaceArea();

    ECollisionChannel GetSurfaceCollisionChannel();
};
