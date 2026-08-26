// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalPlacementAreaComponent.h"

UPortalPlacementAreaComponent::UPortalPlacementAreaComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetGenerateOverlapEvents(false);
	SetHiddenInGame(true);
}