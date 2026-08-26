// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonDoor_Obj.h"
#include "Wire_Obj.h"
#include "Door_Obj.h"
// Sets default values
AButtonDoor_Obj::AButtonDoor_Obj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	m_ButtonBase= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonBase"));
	m_Button= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button"));
	m_Trigger= CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	m_TimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimeLine"));
	SetRootComponent(m_RootComponent);
	m_ButtonBase->SetupAttachment(m_RootComponent);
	m_Button->SetupAttachment(m_ButtonBase);
	m_Trigger->SetupAttachment(m_RootComponent);


	m_bCanUse = true;
	

	m_Trigger->OnComponentBeginOverlap.AddDynamic(this, &AButtonDoor_Obj::Active);
	m_Trigger->OnComponentEndOverlap.AddDynamic(this, &AButtonDoor_Obj::UnActive);
	m_ButtonSound = LoadObject<USoundBase>(nullptr, TEXT("SoundCue'/Game/UportalSystemNiagara/Sounds/SC_button.SC_button'"));
	m_StartSound = LoadObject<USoundBase>(nullptr, TEXT("SoundCue'/Game/UportalSystemNiagara/Sounds/SC_Start.SC_Start'"));
	m_tGlow = LoadObject<UCurveFloat>(nullptr, TEXT("CurveFloat'/Game/Obj/Glow.Glow'"));
	m_tPress = LoadObject<UCurveFloat>(nullptr, TEXT("CurveFloat'/Game/Obj/Press.Press'"));

	FOnTimelineFloat TimelineCallback;
	TimelineCallback.BindUFunction(this, FName("TimelineCallbackFunction"));
	m_TimeLine->AddInterpFloat(m_tGlow, TimelineCallback);
	m_TimeLine->AddInterpFloat(m_tPress, TimelineCallback);
	m_TimeLine->SetTimelineLength(0.2f);
	m_TimeLine->SetPropertySetObject(this);
	
	
}

// Called when the game starts or when spawned
void AButtonDoor_Obj::BeginPlay()
{
	Super::BeginPlay();
	m_ButtonMaterial = m_Button->CreateDynamicMaterialInstance(0);
}

// Called every frame
void AButtonDoor_Obj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AButtonDoor_Obj::Toggle()
{
	m_bActive = true;
	m_ButtonMaterial->SetVectorParameterValue(TEXT("Emissive"), FVector(0.0f, 0.619792f, 0.108922f));
	if (!IsValid(m_ButtonSound))
	{
		m_ButtonSound = LoadObject<USoundBase>(nullptr, TEXT("SoundCue'/Game/UportalSystemNiagara/Sounds/SC_button.SC_button'"));
	}
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_ButtonSound, GetActorLocation());

}

void AButtonDoor_Obj::Active(UPrimitiveComponent* OverlapCom, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (m_bCanUse) {
		
		m_bActive = true;
		if (!IsValid(m_StartSound))
		{
			m_StartSound = LoadObject<USoundBase>(nullptr, TEXT("SoundCue'/Game/UportalSystemNiagara/Sounds/SC_Start.SC_Start'"));
		}
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_StartSound, GetActorLocation());
		m_TimeLine->Play();
		m_Target->Toggle(true);
		if (IsValid(m_Spline)) {
			m_Spline->Glow(true);
		}
	}
}

void AButtonDoor_Obj::UnActive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	if (m_bCanUse) {
		
		m_TimeLine->Reverse();
		m_bActive = false;
		m_Target->Toggle(false);
		if (IsValid(m_Spline)) {
			m_Spline->Glow(false);
		}
	}
}

void AButtonDoor_Obj::TimelineCallbackFunction(float Value)
{
	if (Value <= 0)return;
	m_Button->SetRelativeLocation(FVector(0.f, 0.f, m_tPress->GetFloatValue(Value)));
	
	m_ButtonMaterial->SetVectorParameterValue(TEXT("Emissive"),
		UKismetMathLibrary::LinearColorLerp(FLinearColor(0.f, 0.f, 0.f,1.f), FLinearColor(0.6f, 0.329f, 0.f,1.f), m_tGlow->GetFloatValue(Value)));
}

