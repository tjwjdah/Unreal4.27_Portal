// Fill out your copyright notice in the Description page of Project Settings.


#include "Wire_Obj.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Components/SplineMeshComponent.h"
// Sets default values
AWire_Obj::AWire_Obj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	m_Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	m_tColor = LoadObject<UCurveFloat>(nullptr, TEXT("CurveFloat'/Game/Obj/Color.Color'"));


	SetRootComponent(m_RootComponent);
	m_Spline->SetupAttachment(m_RootComponent);

	
	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance>	Mtrl(TEXT("MaterialInstanceConstant'/Game/UportalSystemNiagara/Materials/Light/MI_Light_Red.MI_Light_Red'"));
	if (Mtrl.Succeeded())
		m_Material = Mtrl.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("StaticMesh'/Game/UportalSystemNiagara/Mesh/SM_wires.SM_wires'"));
	if (Mtrl.Succeeded())
		m_Mesh = Mesh.Object;
	

}

// Called when the game starts or when spawned
void AWire_Obj::BeginPlay()
{
	Super::BeginPlay();
	
	m_Materialins = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), m_Material);
	for (int32 i = 0; i < m_SCp.Num(); i++) {
		m_SCp[i]->SetMaterial(0,m_Materialins);
	}
	FOnTimelineFloat TimelineCallback;
	TimelineCallback.BindDynamic(this, &AWire_Obj::TimelineCallbackFunction);
	m_Timeline.AddInterpFloat(m_tColor, TimelineCallback);
}


void AWire_Obj::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	

	FAttachmentTransformRules Rule(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);
	for (int32 i = m_SCp.Num(); i <= m_Spline->GetNumberOfSplinePoints()-2; i++) {
		USplineMeshComponent* SMesh = NewObject<USplineMeshComponent>(this);
		SMesh->RegisterComponent();
		SMesh->SetStartTangent(FVector(100.f, 0.f, 0.f));
		SMesh->SetEndPosition(FVector(100.f, 0.f, 0.f));
		SMesh->SetEndTangent(FVector(100.f, 0.f, 0.f));;
		SMesh->SetSplineUpDir(FVector(0.f, 0.f, 1.f));
		SMesh->SetStaticMesh(m_Mesh);
		SMesh->SetMaterial(0, m_Materialins);
		SMesh->AttachToComponent(m_RootComponent, Rule);
		m_SCp.Add(SMesh);
		
	}
	
	for (int32 i = 0; i < m_SCp.Num(); i++) {
		
		FVector SLo = FVector::ZeroVector; FVector STa = FVector::ZeroVector; FVector ELo = FVector::ZeroVector; FVector ETa = FVector::ZeroVector;
		m_Spline->GetLocationAndTangentAtSplinePoint(i, SLo, STa, ESplineCoordinateSpace::World);
		m_Spline->GetLocationAndTangentAtSplinePoint(i + 1, ELo, ETa, ESplineCoordinateSpace::World);
		if (IsValid(m_SCp[i])) {
			m_SCp[i]->SetStartAndEnd(SLo, STa, ELo, ETa);
		}
		else {
			USplineMeshComponent* SMesh = NewObject<USplineMeshComponent>(this);
			SMesh->RegisterComponent();
			SMesh->SetStartTangent(FVector(100.f, 0.f, 0.f));
			SMesh->SetEndPosition(FVector(100.f, 0.f, 0.f));
			SMesh->SetEndTangent(FVector(100.f, 0.f, 0.f));;
			SMesh->SetSplineUpDir(FVector(0.f, 0.f, 1.f));
			SMesh->SetStaticMesh(m_Mesh);
			SMesh->SetMaterial(0, m_Materialins);
			SMesh->AttachToComponent(m_RootComponent, Rule);
			m_SCp[i]=SMesh;
			m_SCp[i]->SetStartAndEnd(SLo, STa, ELo, ETa);
		}
	}
}

// Called every frame
void AWire_Obj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	m_Timeline.TickTimeline(DeltaTime);
}

void AWire_Obj::Glow(bool OnOff)
{
	if (OnOff) {
		m_Timeline.Play();
	}
	else {
		m_Timeline.Reverse();
	}
}

void AWire_Obj::TimelineCallbackFunction(float Value)
{
	
	m_Materialins->SetVectorParameterValue(TEXT("Color"),
		UKismetMathLibrary::LinearColorLerp(FLinearColor(1.f, 0.f, 0.1f), FLinearColor(0.01f, 0.6f, 0.f), Value));

}

