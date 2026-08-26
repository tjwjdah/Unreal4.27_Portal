// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "../PortalV2/PortalClone.h"
#include "GameFramework/Actor.h"
#include "BodyClone.generated.h"

UCLASS()
class PP3_API ABodyClone : public APortalClone
{
	GENERATED_BODY()
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* m_Arms;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* m_ArmsGun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* m_Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* m_Body;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* m_Gun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USphereComponent* m_Sphere1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USphereComponent* m_Sphere2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_Grabsocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* m_Capsule;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		//ACameraActor* m_CloneCam;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class APP3Character* m_Original;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UPhysicsHandleComponent* m_PhysicsHandle;

public:
	// Sets default values for this actor's properties
	ABodyClone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetSource(AActor* Actor) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UCameraComponent* GetCamera() { return m_Camera; }
	void SetOriginal(class APP3Character* _Original) { m_Original = _Original; }

	void UpdateAnim();

	UPhysicsHandleComponent* GetPhysicsHandle() { return m_PhysicsHandle; }
	
	USphereComponent* GetSphere2() { return m_Sphere2; }
	virtual void UpdateClone(APortalV2* Portal);
	virtual void HideClear();
	USceneComponent* GetGrabsocket() { return m_Grabsocket; }
};
