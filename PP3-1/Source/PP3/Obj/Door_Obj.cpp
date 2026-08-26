// Fill out your copyright notice in the Description page of Project Settings.


#include "Door_Obj.h"

// Sets default values
ADoor_Obj::ADoor_Obj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(m_RootComponent);
	
	m_RightDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoor"));
	
	m_LeftDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoor"));
	m_RightDoor->SetupAttachment(m_RootComponent);
	m_LeftDoor->SetupAttachment(m_RootComponent);
	m_DoorSound = LoadObject<USoundBase>(nullptr, TEXT("SoundCue'/Game/UportalSystemNiagara/Sounds/SC_BigDoor.SC_BigDoor'"));
	
	m_FLeft.CallbackTarget = this;
	m_FLeft.Linkage = 0;
	m_FLeft.UUID = 0;
	m_FLeft.ExecutionFunction = FName("MoveComponentToFunctionLeft");
	m_FRight.CallbackTarget = this;
	m_FRight.Linkage = 0;
	m_FRight.UUID = 1;
	m_FRight.ExecutionFunction = FName("MoveComponentToFunctionRight");
}

// Called when the game starts or when spawned
void ADoor_Obj::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoor_Obj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoor_Obj::Toggle(bool OnOff)
{
	if (!IsValid(m_DoorSound))
	{
		m_DoorSound = LoadObject<USoundBase>(nullptr, TEXT("SoundCue'/Game/UportalSystemNiagara/Sounds/SC_BigDoor.SC_BigDoor'"));
	}
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_DoorSound, GetActorLocation());
	if (OnOff) {
		if (!m_bMotionLeft)
		{
			
			m_bMotionLeft = true;
			UKismetSystemLibrary::MoveComponentTo(m_LeftDoor, FVector(0.f, 180.f, 0.f), FRotator::ZeroRotator, true, true, 0.2f,
				false,EMoveComponentAction::Move, m_FLeft);
		}
		if (!m_bMotionRight)
		{
			
			m_bMotionRight = true;
			UKismetSystemLibrary::MoveComponentTo(m_RightDoor, FVector(0.f, -180.f, 0.f), FRotator(0.f,180.f,0.f), true, true, 0.2f,
				false, EMoveComponentAction::Move, m_FRight);
		}
	}
	else {
		if (!m_bMotionLeft) {
			m_bMotionLeft = true;
			UKismetSystemLibrary::MoveComponentTo(m_LeftDoor, FVector(0.f, 0.f, 0.f), FRotator::ZeroRotator, true, true, 0.2f,
				false, EMoveComponentAction::Move, m_FLeft);
		}
		if (!m_bMotionRight) {
			m_bMotionRight = true;
			UKismetSystemLibrary::MoveComponentTo(m_RightDoor, FVector(0.f, 0.f, 0.f), FRotator(0.f, -180.f, 0.f), true, true, 0.2f,
				false, EMoveComponentAction::Move, m_FRight);
		}
	}
}

void ADoor_Obj::MoveComponentToFunctionLeft()
{

	if (m_bMotionLeft)
	{
		m_bMotionLeft = false;
		
	}
}

void ADoor_Obj::MoveComponentToFunctionRight()
{
	
	if (m_bMotionRight) {
		m_bMotionRight = false;
	}
}
