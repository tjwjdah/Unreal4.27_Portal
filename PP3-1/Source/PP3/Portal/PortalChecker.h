// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "PortalChecker.generated.h"

UCLASS()
class PP3_API APortalChecker : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		AActor* m_PortalSupport;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_DefaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_UpCheck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_DownCheck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_LeftCheck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_RightCheck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float m_Distance;
	
public:	
	// Sets default values for this actor's properties
	APortalChecker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void SetPortalSupport(AActor* _Actor) {
		m_PortalSupport = _Actor;
	}

	USceneComponent* GetUpCheck() { return m_UpCheck; }
	USceneComponent* GetDownCheck() { return m_DownCheck; }
	USceneComponent* GetLeftCheck() { return m_LeftCheck; }
	USceneComponent* GetRightCheck() { return m_RightCheck; }
};
