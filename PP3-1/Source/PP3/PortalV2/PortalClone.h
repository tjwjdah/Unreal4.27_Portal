// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "PortalClone.generated.h"

class APortalV2;
class UMaterialInstanceDynamic;
UCLASS()
class PP3_API APortalClone : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootScene;

	UPROPERTY(Transient)
	AActor* SourceActor;

	UPROPERTY(Transient)
	TArray<UMaterialInstanceDynamic*> CloneMaterials;
	
public:	
	// Sets default values for this actor's properties
	APortalClone();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void SetSource(AActor* Actor) { SourceActor = Actor; }
	virtual void UpdateClone(APortalV2* Portal);
	void SetClip(APortalV2* Portal, bool bEnable);

	virtual void HideClear() {};
	void AddCloneMaterial(UMaterialInstanceDynamic* Material);

};
