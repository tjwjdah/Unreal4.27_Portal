// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalStaticClone.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"


APortalStaticClone::APortalStaticClone()
{
    CloneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CloneMesh"));
    CloneMesh->SetupAttachment(RootScene);
    CloneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CloneMesh->SetGenerateOverlapEvents(false);
    CloneMesh->SetSimulatePhysics(false);
    CloneMesh->SetCastShadow(false);

    TeleportCloneTag = TEXT("PortalObject");
}

void APortalStaticClone::BeginPlay()
{
    Super::BeginPlay();

}

UStaticMeshComponent* APortalStaticClone::FindSourceMesh(AActor* Actor)
{
    if (!IsValid(Actor))
        return nullptr;

    TArray<UStaticMeshComponent*> MeshComponents;
    Actor->GetComponents<UStaticMeshComponent>(MeshComponents);

    for (UStaticMeshComponent* Mesh : MeshComponents)
    {
        if (Mesh && Mesh->ComponentHasTag(TeleportCloneTag))
            return Mesh;
    }

    UStaticMeshComponent* RootMesh = Cast<UStaticMeshComponent>(Actor->GetRootComponent());

    if (RootMesh)
        return RootMesh;

    return MeshComponents.Num() > 0 ? MeshComponents[0] : nullptr;
}

void APortalStaticClone::SetSource(AActor* Actor)
{
    Super::SetSource(Actor);

    UStaticMeshComponent* SourceMesh = FindSourceMesh(Actor);

    if (!SourceMesh || !SourceMesh->GetStaticMesh())
        return;
   
    CloneMesh->SetStaticMesh(SourceMesh->GetStaticMesh());

    FTransform MeshTransform = SourceMesh->GetComponentTransform().GetRelativeTransform(Actor->GetActorTransform());
    CloneMesh->SetRelativeTransform(MeshTransform);

    const int32 MaterialCount = SourceMesh->GetNumMaterials();

    for (int32 Index = 0; Index < MaterialCount; ++Index)
    {
        UMaterialInterface* SourceMaterial = SourceMesh->GetMaterial(Index);
        CloneMesh->SetMaterial(Index, SourceMaterial);

        UMaterialInstanceDynamic* Material = CloneMesh->CreateDynamicMaterialInstance(Index);

        if (Material)
        {
            AddCloneMaterial(Material);
        }
    }

}
