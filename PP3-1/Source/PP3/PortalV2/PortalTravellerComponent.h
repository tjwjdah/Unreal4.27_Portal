// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "Components/ActorComponent.h"
#include "PortalTravellerComponent.generated.h"



class APortalV2;
class APortalClone;
class UMaterialInstanceDynamic;



struct FSavedPortalCollision
{
	TWeakObjectPtr<UPrimitiveComponent> Primitive;

	ECollisionChannel Channel = ECC_MAX;

	ECollisionResponse Response = ECR_Block;
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPortalTeleport, APortalV2*, APortalV2*);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PP3_API UPortalTravellerComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	FVector PreLocation;


	UPROPERTY(VisibleInstanceOnly)
	TSet<APortalV2*> InPortals;
	


	UPROPERTY(EditAnywhere)
	FName TeleportPrimitiveTag;

	UPROPERTY(VisibleInstanceOnly)
	UPrimitiveComponent* TeleportPrimitive;

	UPROPERTY(EditDefaultsOnly)
	bool bUseClone;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APortalClone> CloneClass;

	UPROPERTY(Transient)
	APortalClone* CloneActor;

	UPROPERTY(Transient)
	APortalV2* CloneSourcePortal;

	UPROPERTY(Transient)
	TArray<UMaterialInstanceDynamic*> OriginalMaterials;

	bool bPortalMoving;
	bool bGrabObject;

	TArray<FSavedPortalCollision> SavedPortalCollisions;
public:
	FOnPortalTeleport OnPortalTeleport;

public:	
	// Sets default values for this component's properties
	UPortalTravellerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	
	void SetPreLocation(const FVector& Vector) { PreLocation = Vector; }
	void SetPreLocation();
	FVector GetPreLocation() { return PreLocation; }

	
	FVector GetOwnerVelocity();

	void SetOwnerVelocity(const FVector& NewVelocity);
	void EnterPortal(APortalV2* Portal);
	void ExitPortal(APortalV2* Portal);

	void UpdatePortalCollisionResponse();

	void RestorePortalCollisionResponse();

	bool IsPhysicsObject();
	UPrimitiveComponent* GetTeleportPrimitive() { return TeleportPrimitive; }
	bool GetPhysicsData(FVector& OutLocation, FQuat& OutRotation, FVector& OutLinearVelocity, FVector& OutAngularVelocity);
	bool SetPhysicsData(const FVector& NewLocation, const FQuat& NewRotation, const FVector& NewLinearVelocity, const FVector& NewAngularVelocity);
	
	FVector GetPortalCheckLocation();
	bool FindTeleportPrimitive();

public:
	void ShowClone(APortalV2* Portal);
	void UpdateClone(APortalV2* Portal);
	void HideClone(APortalV2* Portal);
	void ClearClone();
	void CreateOriginalMaterials();
	void SetOriginalClip(APortalV2* Portal, bool bEnable);

	TSet<APortalV2*>& GetInPortals(){ return InPortals; }
	void SetPortalMoving(bool Moving) { bPortalMoving = Moving; }
	bool GetPortalMoving() { return bPortalMoving; }
	bool IsInPortal(APortalV2* Portal) { return InPortals.Contains(Portal); }

	void SetGrabObject(bool Grap) { bGrabObject = Grap; }
	bool GetGrabObject() { return bGrabObject; }

	APortalClone* GetCloneActor() { return CloneActor; }
	APortalV2* GetCloneSourcePortal() { return CloneSourcePortal; }
};
