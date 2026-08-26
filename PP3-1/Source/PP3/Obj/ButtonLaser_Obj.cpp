// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonLaser_Obj.h"
#include "LaserWall_Obj.h"
// Sets default values
AButtonLaser_Obj::AButtonLaser_Obj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	m_Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	m_ButtonBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonBase"));
	SetRootComponent(m_RootComponent);
	m_Trigger->SetupAttachment(m_RootComponent);
	m_ButtonBase->SetupAttachment(m_RootComponent);

	m_ButtonSound = LoadObject<USoundBase>(nullptr, TEXT("SoundCue'/Game/UportalSystemNiagara/Sounds/SC_button.SC_button'"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>	Mesh(TEXT("StaticMesh'/Game/UportalSystemNiagara/Demo/DemoRoom/Meshes/SM_Button.SM_Button'"));
	if (Mesh.Succeeded())
		m_ButtonBase->SetStaticMesh(Mesh.Object);
	m_ButtonMaterial=m_ButtonBase->CreateDynamicMaterialInstance(2);
	m_Trigger->SetRelativeLocation(FVector(0.f, 75.f, 100.f));
	m_Trigger->SetCapsuleHalfHeight(100.f);
	m_Trigger->SetCapsuleRadius(50.f);
	m_Trigger->OnComponentBeginOverlap.AddDynamic(this, &AButtonLaser_Obj::Active);
	

	m_bFlipFlop = true;
	m_bTriggerTime = true;
}

// Called when the game starts or when spawned
void AButtonLaser_Obj::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AButtonLaser_Obj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (m_bTriggerTime == false)
	{
		m_TriggerTime += DeltaTime;
		if (m_TriggerTime > 0.2f)
		{
			m_bTriggerTime = true;
			m_TriggerTime = 0.f;
		}
	}
}

void AButtonLaser_Obj::Active(UPrimitiveComponent* OverlapCom, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (m_bTriggerTime)m_bTriggerTime = false;
	else {
		return;
	}
	
	Toggle();
}


void AButtonLaser_Obj::Toggle()
{
	

	if (m_bFlipFlop) {
		m_ButtonMaterial->SetScalarParameterValue(TEXT("State"), 1.f);
		m_Target->Toggle(true);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_ButtonSound, GetActorLocation());
		m_bFlipFlop = !m_bFlipFlop;
	}
	else {
		m_ButtonMaterial->SetScalarParameterValue(TEXT("State"), 0.f);
		m_Target->Toggle(false);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_ButtonSound, GetActorLocation());
		m_bFlipFlop = !m_bFlipFlop;
	}
}


