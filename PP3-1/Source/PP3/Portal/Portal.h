// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"


UCLASS()
class PP3_API APortal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APortal();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_DefaultSceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_PortalPlane;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_FakeRecursion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UDecalComponent* m_Decal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_CylinderTeleport;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_CollisionEnabler;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_CollisionDisabler;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_CollisionCloner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneCaptureComponent2D* m_CaptureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_Membrane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UArrowComponent* m_Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		APortal* m_AnotherPortal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_Clone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_ItemClone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		AActor* m_Original;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class APP3Character* m_Player;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Render)
		UTextureRenderTarget2D* m_PortalRenderTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Render)
		UMaterial* m_PortalMaterialInit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Render)
		UMaterial* m_PortalFakeMaterialInit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Render)
		UMaterialInstanceDynamic* m_PortalMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Render)
		UMaterialInstanceDynamic* m_FakeRecursionMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Physic)
		FRotator m_DeltaRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Physic)
		FRotator m_FinalPlayerRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Physic)
		float m_MinimumVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Physic)
		FVector m_SavedVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Physic)
		FVector m_FinalVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Physic)
		FVector m_Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Collision)
		AActor* m_Support;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Collision)
		bool m_bSharedSupport;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Collision)
		TArray<TEnumAsByte<EObjectTypeQuery>> m_SP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Collision)
		TEnumAsByte<ECollisionChannel> m_SupportCollisionObjectType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Collision)
		TEnumAsByte<ECollisionChannel> m_NewSupportCollisionType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Collision)
		TEnumAsByte<ECollisionChannel> m_Channel;




	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = State)
		EPortalType m_Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = State)
		bool m_ReadyToTeleport;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = State)
		bool m_UpStatut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = State)
		bool m_DownStatut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = State)
		bool m_LeftStatut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = State)
		bool m_RightStatut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = State)
		bool m_DestroyAuthorization;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = State)
		bool m_TempDestroyAuthorization;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = State)
		AActor* m_LastTeleportedActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = State)
		int32 m_SpawnNum;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	EPortalType  GetType() {
		return m_Type;
	}
	void SetType(EPortalType _Type) {
		m_Type = _Type;
	}
	APortal* GetAnotherPortal() {
		return m_AnotherPortal;
	}
	void SetAnotherPortal(APortal* _AnotherPortal) {
		m_AnotherPortal = _AnotherPortal;
	}

	void SetPlayer(class APP3Character* _Player) {
		m_Player = _Player;
	}
	

	void SetSupport(AActor* _Support) {
		m_Support = _Support;
	}

	void InitSupportCollisionPreset();

	void SharedSupport();

	void BindOnDestroy();

	void PortalInit(APortal* _portal);

	void InitTagSupport();
	

	ECollisionChannel ConvertCollisionPreset(EObjectTypeQuery _ObjectTypeQuery);

	void UpdateDeltaRotator();
	void UpdatePortalRender();
	

	void TeleportPlayer(ACharacter* _Player);
	void TeleportObjects(AActor* _Actor);
	bool CanTeleport(APortal* _OtherPortal, AActor* _Actor);

	void DestroyOtherPortal(APortal* _OtherPortal);
	void DestroyPortal();

	void UpdatePlayerVelocity(ACharacter* _Player);
	void UpdatePlayerRotation(ACharacter* _Player);
	void PlayerCloneDestroy(ACharacter* _Player);
	void UpdateTeleportationAuthorization();

	void GetFinalVelocity();

	UStaticMeshComponent* GetCylinderTeleport() { return m_CylinderTeleport; }
	FRotator GetDeltaRotator() { return m_DeltaRotator; }
	UArrowComponent* GetArrow() {return m_Arrow;}
	UStaticMeshComponent* GetCollisionCloner(){ return m_CollisionCloner; }
	USceneCaptureComponent2D* GetCaptureComponent() { return m_CaptureComponent; }
	UTextureRenderTarget2D* GetPortalRenderTexture() { return m_PortalRenderTexture; }
	void SetSpawnNum(int32 _Num) { m_SpawnNum = _Num; }

	
	FVector TransformVelocityPortal(const FVector& Velocity);
	FVector TransformLocationPortal(const FVector& Location);
	FQuat TransformRotationPortal(const FQuat& Rotation);
	
	FTransform TransformPortal(const FTransform& Transform);

	
	public:
	UFUNCTION()
		void RemoveClones(AActor* _Act);
	UFUNCTION()
		void Teleport(UPrimitiveComponent* OverlapCom, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void WallPass(UPrimitiveComponent* OverlapCom, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void Clone(UPrimitiveComponent* OverlapCom, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void ResetCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		void ResetClone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
