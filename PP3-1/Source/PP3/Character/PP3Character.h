// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Character.h"
#include "PP3Character.generated.h"

class ABodyClone;
class UPortalTravellerComponent;
class UPortalGunComponent;


UCLASS(config=Game)
class APP3Character : public ACharacter
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = MyCharacter)
	UCameraComponent* m_FirstPersonCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = MyCharacter)
	USkeletalMeshComponent* m_Arms;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Clone)
		bool m_bCloneAlive;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Control)
		bool m_bJumpButtonDown;
	
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* m_Gun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USphereComponent* m_Sphere1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USphereComponent* m_Sphere2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UArrowComponent* m_Arrow1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_Grabsocket;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* m_PortalTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* m_Box;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* m_CharacterMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* m_VisualGun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TEnumAsByte<EGunMode> m_GunMode;
	
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UPhysicsHandleComponent* m_PhysicsHandle;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPortalTravellerComponent* PortalTraveller;

	
	class  UMainHUD* m_MainHUD;



	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPortalGunComponent* PortalGunComponent;
	
	
public:
	APP3Character();



protected:



	void MoveForward(float Value);

	void MoveRight(float Value);

	
	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	
	void Jump();

	void StopJumping();
	void GunModeChange();
	void LMousePressed();
	void RMousePressed();
	void LMouseReleased();
	void RMouseReleased();
	void Fire();
	void Fire2();
	void GrabObject();
	void GrabRelease();
	void GrabLaunch();


protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay() override;
public:
	
	
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return m_FirstPersonCamera; }



	virtual void Tick(float DeltaTime) override;

	bool GetJumpButtonDown() { return m_bJumpButtonDown; }

	UFUNCTION(BlueprintCallable)
	void SetJumpButtonDown(bool _JumpButtonDown) { m_bJumpButtonDown = _JumpButtonDown; }

	

	
	bool GetCloneAlive() { return m_bCloneAlive; }

	USkeletalMeshComponent* GetCharacterMesh() { return m_CharacterMesh;}
	USkeletalMeshComponent* GetVisualGunMesh() { return m_VisualGun; }
	USkeletalMeshComponent* GetArmsMesh() { return m_Arms; }
	USkeletalMeshComponent* GetGunMesh() { return m_Gun; }
	

	USceneComponent* GetGrabSocket() { return m_Grabsocket; }
	UArrowComponent* GetArrow() {return m_Arrow1;}


	void portalvisibletest();

	UPortalGunComponent* GetPortalGunComponent() { return PortalGunComponent; }
};

