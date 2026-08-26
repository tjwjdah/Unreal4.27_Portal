// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "Door_Obj.generated.h"

UCLASS()
class PP3_API ADoor_Obj : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_RootComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_RightDoor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* m_LeftDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USoundBase* m_DoorSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool m_bMotionRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool m_bMotionLeft;

	FLatentActionInfo m_FLeft;
	FLatentActionInfo m_FRight;
public:	
	// Sets default values for this actor's properties
	ADoor_Obj();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	void Toggle(bool OnOff);
	UFUNCTION()
	void MoveComponentToFunctionLeft();
	UFUNCTION()
	void MoveComponentToFunctionRight();
};
