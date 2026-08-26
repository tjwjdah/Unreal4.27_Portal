// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserWall_Obj.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
ALaserWall_Obj::ALaserWall_Obj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_RootComponent= CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	m_Wall = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Wall"));
	m_Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));

	
	m_Timeline= CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	m_OnOff = LoadObject<UCurveFloat>(nullptr, TEXT("CurveFloat'/Game/Obj/OnOff.OnOff'"));
	SetRootComponent(m_RootComponent);
	m_Wall->SetupAttachment(m_RootComponent);
	m_Box->SetupAttachment(m_RootComponent);
	
	FOnTimelineFloat TimelineCallback;
	TimelineCallback.BindUFunction(this, FName("TimelineCallbackFunction"));
	m_Timeline->AddInterpFloat(m_OnOff, TimelineCallback);
	m_Timeline->SetPropertySetObject(this);
	m_Timeline->SetDirectionPropertyName(FName("TimelineDirection"));
	
	m_bEnabled = true;
	m_bActivated = true;
	m_NumberLine = 25;
	m_Lenght = 1350.f;
	m_Divide = 2.f;
	m_SpaceZ = 15.f;

	
	

	
	
	
}

// Called when the game starts or when spawned
void ALaserWall_Obj::BeginPlay()
{
	
	Super::BeginPlay();
	
	m_bEnabled = true;
	m_bActivated = true;
	m_Material = m_Wall->CreateDynamicMaterialInstance(0);

	m_Wall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	m_Material->SetScalarParameterValue(TEXT("Opacity"), 1.0);
	
}

void ALaserWall_Obj::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	for (int32 i = 0; i <= m_NumberLine - 1; i++) {
		BuildWall(i);
	}
	CompleteWall();
}

// Called every frame
void ALaserWall_Obj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALaserWall_Obj::Toggle(bool OnOff)
{
	
	if (m_bEnabled) {
		if (m_bActivated) {
			m_Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			m_bEnabled = !m_bEnabled;
			m_Timeline->PlayFromStart();
		}
	}
	else {
		if (m_bActivated) {
			m_Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			m_bEnabled = !m_bEnabled;
			m_Timeline->Reverse();
		}
	}
	
}

void ALaserWall_Obj::BuildWall(int32 _i)
{
	
	m_Wall->AddInstanceWorldSpace(UKismetMathLibrary::MakeTransform(GetActorForwardVector() * (_i * m_SpaceZ) + m_Wall->GetComponentLocation(), GetActorRotation(),
		FVector(1.f, m_Lenght / 100.f, 1.f)));

		
}

void ALaserWall_Obj::CompleteWall()
{

	m_Box->SetBoxExtent(FVector(((m_SpaceZ * (m_NumberLine - 1)) + m_Divide) / m_Divide, m_Lenght * m_Divide, 6.f));
	m_Box->SetRelativeLocation(FVector(((m_SpaceZ * (m_NumberLine - 1)) + m_Divide) / m_Divide, m_Lenght * m_Divide, 0.f));
}



void ALaserWall_Obj::TimelineCallbackFunction(float _Value)
{
	
	m_Material->SetScalarParameterValue(TEXT("Opacity"), m_OnOff->GetFloatValue(_Value));
}

