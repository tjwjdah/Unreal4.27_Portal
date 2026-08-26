// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalChecker.h"

// Sets default values
APortalChecker::APortalChecker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	m_Distance = 140.f;
	m_DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	m_UpCheck = CreateDefaultSubobject<USceneComponent>(TEXT("UpCheck"));
	m_DownCheck = CreateDefaultSubobject<USceneComponent>(TEXT("DownCheck"));
	m_LeftCheck = CreateDefaultSubobject<USceneComponent>(TEXT("LeftCheck"));
	m_RightCheck = CreateDefaultSubobject<USceneComponent>(TEXT("RightCheck"));

	SetRootComponent(m_DefaultSceneRoot);
	m_UpCheck->SetupAttachment(RootComponent);
	m_DownCheck->SetupAttachment(RootComponent);
	m_LeftCheck->SetupAttachment(RootComponent);
	m_RightCheck->SetupAttachment(RootComponent);
	m_UpCheck->SetRelativeLocation(FVector(0.f, 0.f, 140.f));
	m_DownCheck->SetRelativeLocation(FVector(0.f, 0.f, -140.f));
	m_LeftCheck->SetRelativeLocation(FVector(0.f, 140.f, 0.f));
	m_RightCheck->SetRelativeLocation(FVector(0.f, -140.f, 0.f));



}

// Called when the game starts or when spawned
void APortalChecker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalChecker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

