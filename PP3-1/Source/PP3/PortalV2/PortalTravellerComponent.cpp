// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalTravellerComponent.h"

#include "PortalV2.h"

#include "PortalClone.h"
#include "PortalStaticClone.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/PrimitiveComponent.h"

// Sets default values for this component's properties
UPortalTravellerComponent::UPortalTravellerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	PreLocation = FVector::ZeroVector;
	
	
	TeleportPrimitiveTag = TEXT("PortalObject");
	TeleportPrimitive = nullptr;
	bUseClone = true;
	CloneActor = nullptr;
	CloneSourcePortal = nullptr;
	bPortalMoving = false;
	// ...
}


// Called when the game starts
void UPortalTravellerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
		PreLocation = Owner->GetActorLocation();

	FindTeleportPrimitive();

	CreateOriginalMaterials();
	SetPreLocation();

	// ...
	
}

void UPortalTravellerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearClone();

	InPortals.Empty();
	RestorePortalCollisionResponse();

	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UPortalTravellerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UPortalTravellerComponent::SetPreLocation()
{
	PreLocation = GetPortalCheckLocation();
}



FVector UPortalTravellerComponent::GetOwnerVelocity()
{
	AActor* Owner = GetOwner();
	
	if (!Owner)
		return FVector::ZeroVector;
	ACharacter* Character = Cast<ACharacter>(Owner);
	if (Character)
	{
		UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
		if (Movement)
			return Movement->Velocity;
	}

	return Owner->GetVelocity();
}

void UPortalTravellerComponent::SetOwnerVelocity(const FVector& NewVelocity)
{
	AActor* Owner = GetOwner();

	if (!Owner)
		return;
	ACharacter* Character = Cast<ACharacter>(Owner);
	if (Character)
	{
		UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
		if (Movement)
		{
			if (Movement->IsMovingOnGround() && NewVelocity.Z > KINDA_SMALL_NUMBER)
				Movement->SetMovementMode(MOVE_Falling);
			Movement->Velocity = NewVelocity;
			return;
		}
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* Primitive : PrimitiveComponents)
	{
		if (Primitive && Primitive->IsSimulatingPhysics())
		{
			Primitive->SetPhysicsLinearVelocity(NewVelocity);
		}
	}
}

void UPortalTravellerComponent::EnterPortal(APortalV2* Portal)
{
	if (!IsValid(Portal) || InPortals.Contains(Portal))
		return;
	
	InPortals.Add(Portal);
	
	UpdatePortalCollisionResponse();
}

void UPortalTravellerComponent::ExitPortal(APortalV2* Portal)
{
	
	if (!IsValid(Portal))
		return;
	
	if (InPortals.Remove(Portal) == 0)
		return;
	
	RestorePortalCollisionResponse();
}

void UPortalTravellerComponent::UpdatePortalCollisionResponse()
{
	//RestorePortalCollisionResponse();
	
	TSet<TEnumAsByte<ECollisionChannel>> IgnoreChannels;

	for (APortalV2* Portal : InPortals)
	{
		if (!IsValid(Portal))
			continue;

		ECollisionChannel Channel =Portal->GetSurfaceCollisionChannel();

		if (Channel != ECC_MAX)
			IgnoreChannels.Add(Channel);
	}

	if (IgnoreChannels.Num() == 0)
		return;

	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
		return;

	TArray<UPrimitiveComponent*> Primitives;
	Owner->GetComponents<UPrimitiveComponent>(Primitives);

	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (!IsValid(Primitive) || Primitive->GetCollisionEnabled() ==ECollisionEnabled::NoCollision)
			continue;

		for (auto& Channel :IgnoreChannels)
		{
			FSavedPortalCollision Saved;
			Saved.Response = Primitive->GetCollisionResponseToChannel(Channel);
			if (Saved.Response == ECR_Ignore)
				continue;
			Saved.Primitive = Primitive;
			Saved.Channel = Channel;
			

			SavedPortalCollisions.Add(Saved);

			Primitive->SetCollisionResponseToChannel(Channel,ECR_Ignore);
		}
	}

}

void UPortalTravellerComponent::RestorePortalCollisionResponse()
{
	
	if (GetPortalMoving())
		return;
	for (auto& Saved :SavedPortalCollisions)
	{
		if (!Saved.Primitive.IsValid() ||Saved.Channel == ECC_MAX)
			continue;

		Saved.Primitive->SetCollisionResponseToChannel(Saved.Channel,Saved.Response);
	}

	SavedPortalCollisions.Empty();
}

bool UPortalTravellerComponent::IsPhysicsObject()
{
	return IsValid(TeleportPrimitive) && TeleportPrimitive->IsSimulatingPhysics();
}

bool UPortalTravellerComponent::GetPhysicsData(FVector& OutLocation, FQuat& OutRotation, FVector& OutLinearVelocity, FVector& OutAngularVelocity)
{
	if (!IsPhysicsObject())
		return false;

	OutLocation = TeleportPrimitive->GetComponentLocation();
	OutRotation = TeleportPrimitive->GetComponentQuat();
	OutLinearVelocity = TeleportPrimitive->GetPhysicsLinearVelocity();
	OutAngularVelocity = TeleportPrimitive->GetPhysicsAngularVelocityInRadians();
	return true;
}

bool UPortalTravellerComponent::SetPhysicsData(const FVector& NewLocation, const FQuat& NewRotation, const FVector& NewLinearVelocity, const FVector& NewAngularVelocity)
{
	if (!IsPhysicsObject())
		return false;


	TeleportPrimitive->SetWorldLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
	TeleportPrimitive->SetPhysicsLinearVelocity(NewLinearVelocity);
	TeleportPrimitive->SetPhysicsAngularVelocityInRadians(NewAngularVelocity);
	TeleportPrimitive->WakeAllRigidBodies();

	return true;
}

FVector UPortalTravellerComponent::GetPortalCheckLocation()
{
	AActor* Owner = GetOwner();

	if (!Owner)
		return FVector::ZeroVector;


	
	if (IsPhysicsObject())
		return TeleportPrimitive->GetCenterOfMass();


	return Owner->GetActorLocation();
}

bool UPortalTravellerComponent::FindTeleportPrimitive()
{
	AActor* Owner = GetOwner();

	if (!Owner)
		return false;

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* Primitive : PrimitiveComponents) {
		if (Primitive && Primitive->ComponentHasTag(TeleportPrimitiveTag)) {
			TeleportPrimitive = Primitive;
			return true;
		}
	}


	return false;
}

void UPortalTravellerComponent::ShowClone(APortalV2* Portal)
{
	if (!bUseClone || !IsValid(Portal) || !IsValid(Portal->GetLinkPortal()) || !CloneClass)
		return;

	if (!IsValid(CloneActor))
	{
		UWorld* World = GetWorld();

		if (!World)
			return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CloneActor = World->SpawnActor<APortalClone>(CloneClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (!IsValid(CloneActor))
		{
			ClearClone();
			return;
		}
		CloneActor->SetSource(GetOwner());
	}
	CloneSourcePortal = Portal;
	
	CloneActor->SetActorHiddenInGame(false);
	CloneActor->ShowStart();

	UpdateClone(Portal);
	/*
	ClonePortal = Portal;
	CloneActor->SetActorHiddenInGame(false);
	CloneActor->ShowStart();
	UpdateClone(Portal);
	SetOriginalClip(Portal, true);
	CloneActor->SetClip(Portal, true);
	*/
}

void UPortalTravellerComponent::UpdateClone(APortalV2* Portal)
{
	if (!IsValid(CloneActor) || CloneSourcePortal != Portal)
		return;


	CloneActor->UpdateClone(Portal);
	SetOriginalClip(Portal, true);
	CloneActor->SetClip(Portal, true);
}

void UPortalTravellerComponent::HideClone(APortalV2* Portal)
{
	if (CloneSourcePortal != Portal)
		return;

	SetOriginalClip(nullptr, false);

	if (IsValid(CloneActor))
	{
		CloneActor->SetClip(nullptr, false);
		CloneActor->SetActorHiddenInGame(true);
		CloneActor->OutClone();
	}

	CloneSourcePortal = nullptr;
}

void UPortalTravellerComponent::ClearClone()
{
	SetOriginalClip(nullptr, false);

	if (IsValid(CloneActor))
	{
		CloneActor->HideClear();
		CloneActor->Destroy();
	}

	CloneActor = nullptr;
	CloneSourcePortal = nullptr;
}

void UPortalTravellerComponent::CreateOriginalMaterials()
{
	AActor* Owner = GetOwner();

	if (!Owner)
		return;

	TArray<UMeshComponent*> MeshComponents;

	Owner->GetComponents<UMeshComponent>(MeshComponents);

	for (UMeshComponent* Mesh : MeshComponents)
	{
		if (!Mesh || !Mesh->ComponentHasTag(TeleportPrimitiveTag))
			continue;
		

		const int32 MaterialCount = Mesh->GetNumMaterials();

		for (int32 Index = 0; Index < MaterialCount; ++Index)
		{
			UMaterialInstanceDynamic* Material = Mesh->CreateDynamicMaterialInstance(Index);

			if (Material)
			{
				OriginalMaterials.AddUnique(Material);
			}
		}
	}
}

void UPortalTravellerComponent::SetOriginalClip(APortalV2* Portal, bool bEnable)
{
	for (UMaterialInstanceDynamic* Material : OriginalMaterials)
	{
		if (!Material)
			continue;

		Material->SetScalarParameterValue(TEXT("ClipOn"), bEnable && IsValid(Portal) ? 1.f : 0.f);

		if (bEnable && IsValid(Portal))
		{
			const FVector ClipPoint = Portal->GetPortalLocation();
			const FVector ClipNormal = Portal->GetActorForwardVector().GetSafeNormal();

			Material->SetVectorParameterValue(TEXT("ClipPoint"), FLinearColor(ClipPoint.X, ClipPoint.Y, ClipPoint.Z, 1.f));
			Material->SetVectorParameterValue(TEXT("ClipNormal"), FLinearColor(ClipNormal.X, ClipNormal.Y, ClipNormal.Z, 0.f));
		}
	}
}
