// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "Components/ActorComponent.h"
#include "PortalGunComponent.generated.h"


class APortalV2;
class UPortalSurfaceComponent;
class UPortalTravellerComponent;
class UPhysicsHandleComponent;
class UArrowComponent;
USTRUCT(BlueprintType)
struct FPortalPlacementResult {
	GENERATED_BODY()
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	bool bCanPlace=false;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UPrimitiveComponent* SurfaceComponent = nullptr;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UPortalSurfaceComponent* PortalSurface = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FVector SurfacePoint =FVector::ZeroVector;

	
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PP3_API UPortalGunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPortalGunComponent();

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<APortalV2> InPortalClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APortalV2> OutPortalClass;

	UPROPERTY(VisibleInstanceOnly, Transient)
	APortalV2* InPortal;

	UPROPERTY(VisibleInstanceOnly, Transient)
	APortalV2* OutPortal;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float TraceDistance;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float EdgeMargin;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float PortalOffset;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float CheckStartDistance;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float CheckDepth;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float CheckGap;

	UPROPERTY(EditAnywhere,meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinNormalMatch;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float FrontCheckDepth;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float FrontCheckGap;

	FVector2D PortalHalfSize;



	UPROPERTY(EditDefaultsOnly)
	float GrabTraceDistance = 800.f;

	UPROPERTY(EditDefaultsOnly)
	float PortalTraceOffset = 3.f;

	UPROPERTY(EditDefaultsOnly)
	float ThrowPower = 1500.f;

	UPROPERTY(Transient)
	AActor* GrabbedActor = nullptr;

	UPROPERTY(Transient)
	UPrimitiveComponent* GrabbedComponent = nullptr;

	UPROPERTY(Transient)
	UPortalTravellerComponent* GrabTraveller = nullptr;

	UPROPERTY(Transient)
	UPortalTravellerComponent* PlayerTraveller = nullptr;

	UPROPERTY(Transient)
	APortalV2* GrabPortal = nullptr;

	UPROPERTY(VisibleInstanceOnly)
	bool bGrabThroughPortal = false;

	UPROPERTY(Transient)
	UPhysicsHandleComponent* PhysicsHandle = nullptr;

	UPROPERTY(Transient)
	USceneComponent* GrabSocket =nullptr;

	UPROPERTY(Transient)
	UArrowComponent* ArrowComponent = nullptr;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* Laser;
	UPROPERTY(EditAnywhere)
	UNiagaraComponent* LaserImpact;
	UPROPERTY(EditAnywhere)
	UAudioComponent* LaserSound;
	bool DoOnce;
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraLaser;
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraImpact;

	UPROPERTY(EditAnywhere)
	USoundBase* SoundBase;

	FVector LaserEnd;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CreatePortal();
	void LinkPortal();

	
	FRotator CalculatePortalRotation(const FVector& SurfaceNormal);
	FPortalPlacementResult FindPortalSpot(APortalV2* TargetPortal,APortalV2* OtherPortal);

	void FirePortal(EPortalType PortalType);

	bool PortalInsidePlaceArea(UBoxComponent* PlaceArea,const FVector& SurfacePoint,const FRotator& PortalRotation,FVector& OutSurfacePoint);

	bool PortalSupportCheck(const FVector& SurfacePoint,const FRotator& PortalRotation,UPrimitiveComponent* Surface,APortalV2* TargetPortal,APortalV2* OtherPortal);

	bool IsPortalBlock(const FVector& SurfacePoint, const FRotator& PortalRotation, UPrimitiveComponent* Surface, APortalV2* TargetPortal);

	
	bool GetGrabView(FVector& OutLocation, FVector& OutDirection);

	bool FindGrabHit(FHitResult& OutHit, APortalV2*& OutHitPortal);

	bool CanGrab(const FHitResult& Hit);

	FVector GetGrabTarget();

	void SetGrabPortal(APortalV2* Portal);

	void ClearGrabPortal();

	void ClearGrabData();

	void OnPlayerTeleport(APortalV2* FromPortal, APortalV2* ToPortal);

	void OnGrabObjectTeleport(APortalV2* FromPortal, APortalV2* ToPortal);

	void GrabObject();

	void GrabUpdate();

	void GrabRelease();

	void GrabLaunch();

	void ResetGrabAfterTeleport();

	void PortalBoxComponentsCheck(APortalV2* Portal, UPrimitiveComponent* Surface);

	bool PortalSound();

	bool FireLaser(const FPortalPlacementResult Result, EPortalType PortalType);

	void FireStop();
};
