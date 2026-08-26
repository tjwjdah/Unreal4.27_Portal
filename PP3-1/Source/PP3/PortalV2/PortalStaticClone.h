// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "PortalClone.h"
#include "PortalStaticClone.generated.h"

/**
 * 
 */
UCLASS()
class PP3_API APortalStaticClone : public APortalClone
{
	GENERATED_BODY()


private:
    UPROPERTY(VisibleDefaultsOnly)
    UStaticMeshComponent* CloneMesh;

	UPROPERTY(EditAnywhere)
	FName TeleportCloneTag;
public:
	// Sets default values for this actor's properties
	APortalStaticClone();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    UStaticMeshComponent* FindSourceMesh(AActor* Actor);


    virtual void SetSource(AActor* Actor) override;
};
