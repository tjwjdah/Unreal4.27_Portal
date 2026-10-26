// Copyright Epic Games, Inc. All Rights Reserved.

#include "PP3Character.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"

#include "Components/BoxComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "../Obj/Grab_Obj.h"
#include "../PP3GameMode.h"
#include "BodyClone.h"

#include "../PortalV2/PortalGunComponent.h"
#include "../PortalV2/PortalTravellerComponent.h"
// APP3Character

APP3Character::APP3Character()
{

	m_FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	m_Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	m_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	m_Sphere1 = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere1"));
	m_Sphere2 = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere2"));
	m_Arrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow1"));
	m_PortalTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PortalTrigger"));
	m_Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	m_CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	m_VisualGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VisualGun"));
	m_PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	m_Grabsocket = CreateDefaultSubobject<USceneComponent>(TEXT("Grabsocket"));
	PortalTraveller = CreateDefaultSubobject<UPortalTravellerComponent>(TEXT("PortalTraveller"));
	PortalGunComponent = CreateDefaultSubobject<UPortalGunComponent>(TEXT("PortalGunComponent"));
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	m_FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	m_FirstPersonCamera->bUsePawnControlRotation = true;
	


	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;

	m_Arms->SetupAttachment(m_FirstPersonCamera);

	
	m_Gun->SetupAttachment(m_Arms);
	
	
	m_Sphere1->SetupAttachment(m_Gun);
	m_Sphere2->SetupAttachment(m_Gun);
	
	m_Arrow1->SetupAttachment(m_Sphere1);
	m_Grabsocket->SetupAttachment(m_Arrow1);
	
	m_PortalTrigger->SetupAttachment(m_Sphere1);

	
	m_Box->SetupAttachment(m_FirstPersonCamera);

	
	m_CharacterMesh->SetupAttachment(GetCapsuleComponent());

	
	m_VisualGun->SetupAttachment(m_CharacterMesh,TEXT("Gun_Socket"));

	
	
	m_bJumpButtonDown = false;
	m_GunMode = EGunMode::Portal;
}


void APP3Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APP3Character::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APP3Character::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APP3Character::LMousePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APP3Character::LMouseReleased);
	PlayerInputComponent->BindAction("Fire2", IE_Pressed, this, &APP3Character::RMousePressed);
	PlayerInputComponent->BindAction("Change", IE_Pressed, this, &APP3Character::GunModeChange);
	
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &APP3Character::portalvisibletest);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &APP3Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APP3Character::MoveRight);

	
	PlayerInputComponent->BindAxis("Turn", this, &APP3Character::TurnAtRate);
	
	PlayerInputComponent->BindAxis("LookUp", this, &APP3Character::LookUpAtRate);
	

}

void APP3Character::BeginPlay()
{
	Super::BeginPlay();

	if (APP3GameMode* GameMode = Cast<APP3GameMode>(GetWorld()->GetAuthGameMode()))
		m_MainHUD = GameMode->GetMainHUD();

	

}

void APP3Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	

}

void APP3Character::portalvisibletest()
{
	if (IsValid(PortalGunComponent))
		PortalGunComponent->portalvisibletest();
}


void APP3Character::Jump()
{
	Super::Jump();
}

void APP3Character::StopJumping()
{
	Super::StopJumping();
}

void APP3Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate);
}

void APP3Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate);

	//m_FirstPersonCamera->SetWorldRotation(GetControlRotation());
}

void APP3Character::MoveForward(float Value)
{
	if (!Controller || FMath::IsNearlyZero(Value))
		return;
	
	const FVector Direction = GetActorForwardVector();
	AddMovementInput(Direction, Value);
	
}

void APP3Character::MoveRight(float Value)
{
	if (!Controller || FMath::IsNearlyZero(Value))
		return;
		
	const FVector Direction = GetActorRightVector();
	AddMovementInput(Direction, Value);

}



void APP3Character::GrabObject()
{

	if (IsValid(PortalGunComponent))
		PortalGunComponent->GrabObject();

}

void APP3Character::GrabRelease()
{

	if (IsValid(PortalGunComponent))
		PortalGunComponent->GrabRelease();
	
	
}

void APP3Character::GrabLaunch()
{
	if (IsValid(PortalGunComponent))
		PortalGunComponent->GrabLaunch();


}


void APP3Character::Fire()
{
	if (PortalGunComponent)
		PortalGunComponent->FirePortal(EPortalType::PIN);
	
}



void APP3Character::Fire2()
{
	if (PortalGunComponent)
		PortalGunComponent->FirePortal(EPortalType::POUT);
	
}


void APP3Character::GunModeChange()
{
	if (m_GunMode == EGunMode::Grab && IsValid(PortalGunComponent))
		PortalGunComponent->GrabRelease();

	if (m_GunMode == EGunMode::Portal)
		m_GunMode = EGunMode::Grab;
	else
		m_GunMode = EGunMode::Portal;

	if (m_MainHUD && m_MainHUD->GetGunMode())
		m_MainHUD->GetGunMode()->SetMode(m_GunMode);
}


void APP3Character::LMousePressed()
{

	if (m_GunMode == EGunMode::Portal)
	{
		Fire();
	}
	else {
		GrabObject();
	}
}

void APP3Character::RMousePressed()
{
	if (m_GunMode == EGunMode::Portal)
	{
		Fire2();
	}
	else {
		GrabLaunch();
	}
}
void APP3Character::LMouseReleased()
{
	if (m_GunMode == EGunMode::Grab)
		GrabRelease();
}

