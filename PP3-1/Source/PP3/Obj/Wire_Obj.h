// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "Wire_Obj.generated.h"

UCLASS()
class PP3_API AWire_Obj : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent* m_RootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USplineComponent* m_Spline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UStaticMesh* m_Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UMaterialInstanceDynamic* m_Materialins;

	UPROPERTY(EditAnywhere)
		FTimeline m_Timeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UCurveFloat* m_tColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TEnumAsByte<ETimelineDirection::Type> TimelineDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TArray<class USplineMeshComponent*> m_SCp;
	UMaterialInstance* m_Material;
public:	
	// Sets default values for this actor's properties
	AWire_Obj();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	void Glow(bool OnOff);

	UFUNCTION()
		void TimelineCallbackFunction(float Value);
};

