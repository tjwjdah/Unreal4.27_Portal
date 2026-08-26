// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "Grab_Obj.generated.h"



UCLASS()
class PP3_API AGrab_Obj : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrab_Obj();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FLinearColor m_LaserColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UMaterialInstanceDynamic* m_MaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UMaterial* m_Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool m_Grabbing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Portal)
		class APortal* m_WalkThroughPortal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Portal)
		bool m_bInsidePortal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Clone)
		AGrab_Obj* m_Clone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Clone)
		bool m_bCloneAlive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Clone)
		float m_ClonePitchInit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Clone)
		bool m_bDotResult;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AGrab_Obj> m_CloneBp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Clone)
	class UPortalTravellerComponent* PortalTraveller;

	bool m_bIClone;
	bool m_bPortalInGrab;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



public:

	void SetForceDirection(FVector _Dir, float _Force);

	bool GetGrabbing() { return m_Grabbing; }
	void SetGrabbing(bool _Grabbing) { m_Grabbing = _Grabbing; }
	bool GetPortalInGrab() { return m_bPortalInGrab; }
	void SetPortalInGrab(bool _PortalInGrab) { m_bPortalInGrab = _PortalInGrab; }
	
	void SetWalkThroughtPortal(APortal* _WalkThroughtPortal) { m_WalkThroughPortal = _WalkThroughtPortal; }
	
	void CloneManagement();
	void SetCloneAlive(bool _CloneAlive) { m_bCloneAlive = _CloneAlive; }
	void MaterialSet();
	AGrab_Obj* GetClone() { if (IsValid(m_Clone)) { return m_Clone; } else return nullptr; }

	UStaticMeshComponent* GetStaticMesh() { return m_StaticMesh; }

	void CloneDestroy();
};
