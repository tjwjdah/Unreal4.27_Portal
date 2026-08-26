// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalV2.h"
#include "PortalTravellerComponent.h"
#include "PortalSurfaceComponent.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/DecalComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneCaptureComponent2D.h"

#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraTypes.h"
#include "Math/RotationMatrix.h"
#include "Math/TranslationMatrix.h"

#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Components/PrimitiveComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Components/CapsuleComponent.h"

#include "Engine/TextureRenderTarget2D.h"


// Sets default values

void FPortalRenderTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
    if (Target && Target->GetActive())
        Target->UpdatePortalRender();
}

FString FPortalRenderTickFunction::DiagnosticMessage()
{
    return TEXT("FPortalRenderTickFunction");
}

APortalV2::APortalV2()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;


    PortalRenderTick.Target = this;
    PortalRenderTick.bCanEverTick = true;
    PortalRenderTick.bStartWithTickEnabled = true;
    PortalRenderTick.TickGroup = TG_PostUpdateWork;

    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    SetRootComponent(RootSceneComponent);

    RootVisual = CreateDefaultSubobject<USceneComponent>(TEXT("RootVisual"));
    RootVisual->SetupAttachment(RootSceneComponent);

    PortalPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalPlane"));
    PortalPlane->SetupAttachment(RootVisual);
    PortalPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PortalInner = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalInner"));
    PortalInner->SetupAttachment(RootVisual);
    PortalInner->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PortalNearClipShell = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalNearClipShell"));
    PortalNearClipShell->SetupAttachment(RootVisual);
    PortalNearClipShell->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    PortalNearClipShell->SetGenerateOverlapEvents(false);
    PortalNearClipShell->SetCastShadow(false);
    PortalNearClipShell->SetCanEverAffectNavigation(false);

    PortalDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("PortalDecal"));
    PortalDecal->SetupAttachment(RootVisual);

    PortalEnterTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PortalEnterTrigger"));
    PortalEnterTrigger->SetupAttachment(RootSceneComponent);
    PortalEnterTrigger->SetBoxExtent(FVector(100.f, 90.f, 130.f));
    PortalEnterTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PortalEnterTrigger->SetCollisionObjectType(ECC_WorldDynamic);
    PortalEnterTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    PortalEnterTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    PortalEnterTrigger->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
    PortalEnterTrigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    PortalEnterTrigger->SetGenerateOverlapEvents(true);

    PortalCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("PortalCamera"));
    PortalCamera->SetupAttachment(RootSceneComponent);
    PortalCamera->CaptureSource = SCS_SceneColorHDR;
    PortalCamera->CompositeMode = SCCM_Overwrite;
    PortalCamera->bCaptureEveryFrame = false;
    PortalCamera->bCaptureOnMovement = false;
    PortalCamera->bAlwaysPersistRenderingState = false;
    PortalCamera->bEnableClipPlane = true;
    PortalCamera->bUseCustomProjectionMatrix = true;
    PortalCamera->ShowFlags.SetMotionBlur(false);
    PortalCamera->ShowFlags.SetEyeAdaptation(false);
    PortalCamera->ShowFlags.SetBloom(false);
    
    DirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));
    DirectionArrow->SetupAttachment(RootSceneComponent);


    for (int32 i = 0; i < 30; i++) {
        FName PieceName;
        if (i < 10)
            PieceName = *FString::Printf(TEXT("PortalBlockLeft%d"), i + 1);
        else if (i >= 10 && i < 20)
            PieceName = *FString::Printf(TEXT("PortalBlockRight%d"), i - 9);
        else if (i >= 20 && i < 25)
            PieceName = *FString::Printf(TEXT("PortalBlockTop%d"), i - 19);
        else if (i >= 25 && i < 30)
            PieceName = *FString::Printf(TEXT("PortalBlockBottom%d"), i - 24);
        UBoxComponent* PortalBlock = CreateDefaultSubobject<UBoxComponent>(PieceName);
        PortalBlock->SetupAttachment(RootVisual);
        PortalBlock->SetCollisionProfileName(TEXT("BlockAllDynamic"));
        PortalBlock->ComponentTags.Add(TEXT("PortalBlockBox"));
        PortalBlocks.Add(PortalBlock);
        if (i < 10)
        {
            PortalBlock->SetBoxExtent(FVector(1, 1, 130));
            PortalBlock->SetRelativeLocation(FVector(0.f, 91.f + i * 2, 0));
        }
        else if (i >= 10 && i < 20)
        {
            PortalBlock->SetBoxExtent(FVector(1, 1, 130));
            PortalBlock->SetRelativeLocation(FVector(0.f, -91.f - (i - 10) * 2, 0));
        }
        else if (i >= 20 && i < 25) {
            PortalBlock->SetBoxExtent(FVector(1, 92, 1));
            PortalBlock->SetRelativeLocation(FVector(0.f, 0.f, 131.f + (i - 20) * 2));
        }
        else if (i >= 25 && i < 30) {
            PortalBlock->SetBoxExtent(FVector(1, 92, 1));
            PortalBlock->SetRelativeLocation(FVector(0.f, 0.f, -131.f - (i - 25) * 2));
        }
    }



    LinkPortal = nullptr;
    PortalRenderTarget = nullptr;

    PortalPlaneMaterialInstance = nullptr;
    PortalInnerMaterialInstance = nullptr;
    PortalDecalMaterialInstance = nullptr;
    PortalNearClipShellMaterialInstance = nullptr;
    bActive = false;

    static ConstructorHelpers::FObjectFinder<UMaterial>	Mtrl(TEXT("Material'/Game/Portal/Materials/M_PortalDecal_Glow.M_PortalDecal_Glow'"));
    if (Mtrl.Succeeded())
        PortalDecalMaterial = Mtrl.Object;
    RenderTargetWidth = 1920;
    RenderTargetHeight = 1080;


    ExitOffset = 10.f;
    PortalHolePadding = 0.25f;
    PortalHalfSize = FVector2D(90.f, 130.f);
    TravellerMargin = 2.f;

}

// Called when the game starts or when spawned
void APortalV2::BeginPlay()
{
    Super::BeginPlay();


    PortalEnterTrigger->OnComponentBeginOverlap.AddDynamic(this, &APortalV2::OnPortalTriggerBegin);
    PortalEnterTrigger->OnComponentEndOverlap.AddDynamic(this, &APortalV2::OnPortalTriggerEnd);

    InitializePortalRenderTarget();
    CreatePortalRenderTargets();
    InitializePortalMaterials();
    // ActivatePortal();
    ResetPortalRenderHistory();
    DeActivatePortal();
  
}

void APortalV2::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

    ClearTravellers();
    ClearSurfaceHole();
    Super::EndPlay(EndPlayReason);
}

void APortalV2::RegisterActorTickFunctions(bool bRegister)
{
    Super::RegisterActorTickFunctions(bRegister);

    if (bRegister)
    {
        PortalRenderTick.Target = this;

        if (!PortalRenderTick.IsTickFunctionRegistered())
        {
            PortalRenderTick.SetTickFunctionEnable(true);
            PortalRenderTick.RegisterTickFunction(GetLevel());
        }
    }
    else
    {
        if (PortalRenderTick.IsTickFunctionRegistered())
            PortalRenderTick.UnRegisterTickFunction();

        PortalRenderTick.Target = nullptr;
    }
}

// Called every frame
void APortalV2::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bActive)
        return;

    if (!IsValid(LinkPortal) || !LinkPortal->GetActive())
    {
        InvalidatePortalHistory();
        return;
    }



    TArray<AActor*> Actors = OverlappingActors.Array();

    for (auto* Actor : Actors) {
        if (!IsValid(Actor)) {
            OverlappingActors.Remove(Actor);
            continue;
        }

        if (!IsValid(LinkPortal) || !LinkPortal->GetActive())
            continue;
        UPortalTravellerComponent* Traveller = Actor->FindComponentByClass<UPortalTravellerComponent>();

        if (!Traveller)
        {
            OverlappingActors.Remove(Actor);
            continue;
        }


        FVector PreLocation = Traveller->GetPreLocation();
        FVector CurrentLocation = Traveller->GetPortalCheckLocation();

        Traveller->UpdateClone(this);

        FVector CrossingPoint;

        if (FindCrossPoint(PreLocation, CurrentLocation, CrossingPoint))
        {

            if (!IsPointInsideOpening(CrossingPoint))
            {
                Traveller->SetPreLocation(CurrentLocation);
                continue;
            }

            if (PortalTeleport(Actor))
                continue;

         
        }

        Traveller->SetPreLocation(CurrentLocation);
    }

    for (auto It = OverlappingActors.CreateIterator(); It; ++It)
    {
        if (!IsValid(*It))
            It.RemoveCurrent();
    }

  
}

void APortalV2::InitializePortalRenderTarget()
{
    UWorld* World = GetWorld();
    if (!World)
        return;
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (PC)
        PC->GetViewportSize(RenderTargetWidth, RenderTargetHeight);


    if (RenderTargetWidth <= 0 || RenderTargetHeight <= 0)
    {
        RenderTargetWidth = 1920;
        RenderTargetHeight = 1080;
    }

    PortalRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(World, RenderTargetWidth, RenderTargetHeight, ETextureRenderTargetFormat::RTF_RGBA16f);

    if (!PortalRenderTarget) return;

    PortalRenderTarget->ClearColor = FLinearColor::Black;
    PortalRenderTarget->Filter = TF_Bilinear;
    PortalRenderTarget->AddressX = TA_Clamp;
    PortalRenderTarget->AddressY = TA_Clamp;
    PortalRenderTarget->UpdateResourceImmediate(true);

    if (PortalCamera)
        PortalCamera->TextureTarget = PortalRenderTarget;
}

void APortalV2::InitializePortalMaterials()
{
    if (PortalPlaneMaterial)
        PortalPlane->SetMaterial(0, PortalPlaneMaterial);

    if (PortalInnerMaterial)
        PortalInner->SetMaterial(0, PortalInnerMaterial);

    if (PortalDecal && PortalDecalMaterial)
    {
        PortalDecalMaterialInstance = UMaterialInstanceDynamic::Create(PortalDecalMaterial, this);

        PortalDecal->SetDecalMaterial(PortalDecalMaterialInstance);
    }

    PortalPlaneMaterialInstance = PortalPlane->CreateDynamicMaterialInstance(0);
    PortalInnerMaterialInstance = PortalInner->CreateDynamicMaterialInstance(0);


    if (PortalPlaneMaterialInstance && PortalRenderTarget) {
        PortalPlaneMaterialInstance->SetTextureParameterValue(TEXT("ConnectedRender"), PortalRenderTarget);
        PortalPlaneMaterialInstance->SetScalarParameterValue(TEXT("DeepOverscanScale"), FMath::Max(1.f, HistoryOverscanScale));
        PortalPlaneMaterialInstance->SetScalarParameterValue(TEXT("UseDeepOverscanMapping"), 0.f);
    }

    if (PortalInnerMaterialInstance) {
        PortalInnerMaterialInstance->SetVectorParameterValue(TEXT("InnerColor"), InnerColor);
    }

    if (PortalDecalMaterialInstance) {
        FLinearColor FinalDecalColor = PortalColor;
        PortalDecalMaterialInstance->SetVectorParameterValue(TEXT("DecalColor"), FinalDecalColor);
    }

    if (PortalNearClipShell->GetMaterial(0))
        PortalNearClipShellMaterialInstance = PortalNearClipShell->CreateDynamicMaterialInstance(0);

    if (PortalNearClipShellMaterialInstance && PortalRenderTarget) {
        PortalNearClipShellMaterialInstance->SetTextureParameterValue(TEXT("ConnectedRender"), PortalRenderTarget);
        PortalNearClipShellMaterialInstance->SetScalarParameterValue(TEXT("DeepOverscanScale"), FMath::Max(1.f, HistoryOverscanScale));
        PortalNearClipShellMaterialInstance->SetScalarParameterValue(TEXT("UseDeepOverscanMapping"), 0.f);
    }
    if (PortalRecurMaterial)
    {
        PortalRecurMaterialInstance = UMaterialInstanceDynamic::Create(PortalRecurMaterial, this);

        if (PortalRecurMaterialInstance)
        {
            PortalRecurMaterialInstance->SetTextureParameterValue(TEXT("ConnectedRender"), PortalRecurRenderTarget);

            PortalRecurMaterialInstance->SetVectorParameterValue(TEXT("RecurFallbackColor"), PortalColor);

            PortalRecurMaterialInstance->SetScalarParameterValue(TEXT("UseHistory"), 0.f);
        }
    }

}



void APortalV2::ActivatePortal()
{
    bActive = true;

    SetActorHiddenInGame(false);
    SetActorTickEnabled(true);
    PortalRenderTick.SetTickFunctionEnable(true);
    if (PortalPlane)
        PortalPlane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    if (PortalEnterTrigger)
        PortalEnterTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    if (PortalCamera) {
        PortalCamera->SetComponentTickEnabled(true);
        PortalCamera->bEnableClipPlane = true;
    }
    
    AllPortalBlocksBlockOn(true);

    HiddenComponentsUpdate();
}

void APortalV2::DeActivatePortal()
{
    InvalidatePortalHistory();

    ClearTravellers();
    ClearSurfaceHole();

    bActive = false;

    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    PortalRenderTick.SetTickFunctionEnable(false);
    if (PortalPlane)
        PortalPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (PortalEnterTrigger)
        PortalEnterTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (PortalCamera) {
        PortalCamera->SetComponentTickEnabled(false);
        PortalCamera->bEnableClipPlane = false;
    }
   
    AllPortalBlocksBlockOn(false);

}

void APortalV2::SetLinkPortal(APortalV2* InLinkPortal)
{
    LinkPortal = InLinkPortal;
    ResetPortalRenderHistory();
}


FVector APortalV2::GetPortalLocation()
{
    if (PortalPlane) {
        return PortalPlane->GetComponentLocation();
    }
    return GetActorLocation();
}



void APortalV2::OnPortalTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!IsTravellerBody(OtherActor, OtherComp))
        return;

    UPortalTravellerComponent* Traveller =
        OtherActor->FindComponentByClass<UPortalTravellerComponent>();

    if (!IsValid(Traveller) || Traveller->GetPortalMoving())
        return;
    AddTraveller(OtherActor);
}

void APortalV2::OnPortalTriggerEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{


    if (!IsTravellerBody(OtherActor, OtherComp))
        return;

    UPortalTravellerComponent* Traveller = OtherActor->FindComponentByClass<UPortalTravellerComponent>();

    if (!IsValid(Traveller) || Traveller->GetPortalMoving())
        return;
    RemoveTraveller(OtherActor);
}

void APortalV2::AddTraveller(AActor* Actor)
{
    if (!bActive || !IsValid(LinkPortal) || !LinkPortal->GetActive())
        return;
    if (!IsValid(Actor) || Actor == this)
        return;

    if (OverlappingActors.Contains(Actor))
        return;

    UPortalTravellerComponent* Traveller = Actor->FindComponentByClass<UPortalTravellerComponent>();

    if (!Traveller)
        return;


    OverlappingActors.Add(Actor);
    Traveller->EnterPortal(this);
    Traveller->SetPreLocation();
    Traveller->ShowClone(this);


}

void APortalV2::RemoveTraveller(AActor* Actor)
{
    if (!IsValid(Actor))
        return;

    UPortalTravellerComponent* Traveller = Actor->FindComponentByClass<UPortalTravellerComponent>();

    bool bWasTracked = OverlappingActors.Remove(Actor) > 0;
    if (!IsValid(Traveller))
        return;

    bool bWasInside = Traveller->IsInPortal(this);

    if (!bWasTracked && !bWasInside)
        return;


    Traveller->ExitPortal(this);
    Traveller->HideClone(this);

  
}

FVector APortalV2::TransformPoint(const FVector& WorldPoint)
{
    if (!IsValid(LinkPortal))
        return WorldPoint;
    FVector SrcLocation = GetPortalLocation();
    FVector DestLocation = LinkPortal->GetPortalLocation();

    FVector SrcToPoint = WorldPoint - SrcLocation;

    FVector DestOffset = TransformVector(SrcToPoint);

    return DestLocation + DestOffset;

}

FVector APortalV2::TransformVector(const FVector& WorldVector)
{
    if (!IsValid(LinkPortal))
        return WorldVector;
    FVector SrcForward = GetActorForwardVector();
    FVector SrcRight = GetActorRightVector();
    FVector SrcUp = GetActorUpVector();


    FVector DestForward = -LinkPortal->GetActorForwardVector();
    FVector DestRight = -LinkPortal->GetActorRightVector();
    FVector DestUp = LinkPortal->GetActorUpVector();

    FVector LocalCoordinate;
    LocalCoordinate.X = FVector::DotProduct(WorldVector, SrcForward);
    LocalCoordinate.Y = FVector::DotProduct(WorldVector, SrcRight);
    LocalCoordinate.Z = FVector::DotProduct(WorldVector, SrcUp);

    return LocalCoordinate.X * DestForward + LocalCoordinate.Y * DestRight + LocalCoordinate.Z * DestUp;
}

FQuat APortalV2::TransformRotation(const FQuat& WorldRotation)
{
    if (!IsValid(LinkPortal))
        return WorldRotation;




    FQuat Quat = GetActorQuat();
    FQuat DestinationQuat = LinkPortal->GetActorQuat();

    FQuat Diff = DestinationQuat * Quat.Inverse();
    FQuat Result = Diff * WorldRotation;


    FQuat HalfTurn(LinkPortal->GetActorUpVector(), PI);

    return (HalfTurn * Result).GetNormalized();

}

bool APortalV2::PortalTeleport(AActor* Actor)
{
    if (!IsValid(Actor) || !IsValid(LinkPortal))
        return false;
    UPortalTravellerComponent* Traveller = Actor->FindComponentByClass<UPortalTravellerComponent>();

    if (!Traveller)
        return false;
    UWorld* World = GetWorld();
    if (!World)
        return false;

    Traveller->SetPortalMoving(true);

    bool bTeleport = Traveller->IsPhysicsObject() ? TeleportObjects(Actor, Traveller) : TeleportPlayer(Actor, Traveller);

    if (!bTeleport)
    {
        Traveller->SetPortalMoving(false);
        return false;
    }

    InvalidatePortalHistory();
    if (IsValid(LinkPortal))
        LinkPortal->InvalidatePortalHistory();

    RemoveTraveller(Actor);
    LinkPortal->AddTraveller(Actor);
    Traveller->SetPreLocation();
    Traveller->OnPortalTeleport.Broadcast(this, LinkPortal);
    Traveller->UpdateClone(LinkPortal);
    Traveller->SetPortalMoving(false);
    return true;
}

void APortalV2::PlaceSurface(const FVector& NewLocation, const FRotator& NewRotation, const FVector& SurfacePoint, UPortalSurfaceComponent* NewSurface)
{
    if (!NewSurface)
        return;
    ClearTravellers();

    ClearSurfaceHole();

    SetActorLocationAndRotation(NewLocation, NewRotation);
    ResetPortalRenderHistory();

    if (IsValid(LinkPortal))
        LinkPortal->ResetPortalRenderHistory();

    FVector Forward = GetActorForwardVector();

    if (FMath::Abs(Forward.Z) > 0.9f)
        PortalPlane->SetVisibility(true);
    else
        PortalPlane->SetVisibility(false);
    CurrentSurfacePoint = SurfacePoint;
    CurrentPortalSurface = NewSurface;
    AddSurfaceHole();
    ActivatePortal();
}

void APortalV2::AddSurfaceHole()
{
    if (!CurrentPortalSurface)
        return;

    FVector Forward = GetActorForwardVector().GetSafeNormal();

    FVector Right = GetActorRightVector().GetSafeNormal();

    FVector Up = GetActorUpVector().GetSafeNormal();

    float SurfaceDepth = CurrentPortalSurface->GetSurfaceDepth();

    FPortalHoleData HoleData;

    HoleData.Center = CurrentSurfacePoint - Forward * (SurfaceDepth * 0.5f);
    HoleData.Forward = Forward;
    HoleData.Right = Right;
    HoleData.Up = Up;
    HoleData.HalfWidth = PortalHalfSize.X + PortalHolePadding;
    HoleData.HalfHeight = PortalHalfSize.Y + PortalHolePadding;

    HoleData.HalfDepth = SurfaceDepth * 0.5f + 1.f;

    HoleData.bEnabled = true;

    CurrentPortalSurface->SetPortalHole(PortalType, HoleData);
}

void APortalV2::ClearSurfaceHole()
{
    if (!CurrentPortalSurface)
        return;

    CurrentPortalSurface->ClearPortalHole(PortalType);
    CurrentPortalSurface = nullptr;
}

void APortalV2::HiddenComponentsUpdate()
{
    if (!PortalCamera)
        return;


    if (IsValid(LinkPortal))
    {
        PortalCamera->HideComponent(LinkPortal->PortalPlane);
        PortalCamera->HideComponent(LinkPortal->PortalNearClipShell);
        PortalCamera->HideComponent(LinkPortal->PortalInner);
    
    }

}

bool APortalV2::FindCrossPoint(const FVector& PreviousLocation, const FVector& CurrentLocation, FVector& OutPoint)
{
    FVector PortalLocation = GetPortalLocation();
    FVector PortalForward = GetActorForwardVector().GetSafeNormal();

    float PreviousSide = FVector::DotProduct(PreviousLocation - PortalLocation, PortalForward);
    float CurrentSide = FVector::DotProduct(CurrentLocation - PortalLocation, PortalForward);

    if (!(PreviousSide > 0.f && CurrentSide <= 0.f)) {

        return false;
    }
    float Denominator = PreviousSide - CurrentSide;

    if (FMath::IsNearlyZero(Denominator))
        return false;

    float Alpha = FMath::Clamp(PreviousSide / Denominator, 0.f, 1.f);
    OutPoint = FMath::Lerp(PreviousLocation, CurrentLocation, Alpha);

    return true;
}

bool APortalV2::TeleportPlayer(AActor* Actor, UPortalTravellerComponent* Traveller)
{
    FVector OldLocation = Actor->GetActorLocation();
    FQuat OldRotation = Actor->GetActorQuat();
    FVector OldVelocity = Traveller->GetOwnerVelocity();

    FVector NewLocation = TransformPoint(OldLocation);
    FQuat NewRotation = TransformRotation(OldRotation);
    FVector NewVelocity = TransformVector(OldVelocity);

    if (!NewVelocity.IsNearlyZero())
        NewLocation += NewVelocity.GetSafeNormal() * ExitOffset;
    else
        NewLocation += LinkPortal->GetActorForwardVector() * ExitOffset;

    ACharacter* Character = Cast<ACharacter>(Actor);

    if (Character) {
        Character->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
        if (AController* Controller = Character->GetController())
        {
            FQuat OldControlQuat =
                Controller->GetControlRotation().Quaternion();

            FQuat NewControlQuat = TransformRotation(OldControlQuat);

            Controller->SetControlRotation(NewControlQuat.Rotator());
        }
        FRotator Rotator(0.0, NewRotation.Rotator().Yaw, 0.0);

        Character->SetActorRotation(Rotator);

    }
    else {
        Actor->SetActorLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
    }
    Traveller->SetOwnerVelocity(NewVelocity);
    return true;
}

void APortalV2::ClearTravellers()
{
    TArray<AActor*> Actors = OverlappingActors.Array();

    for (AActor* Actor : Actors)
    {
        if (IsValid(Actor))
            RemoveTraveller(Actor);
    }

    OverlappingActors.Empty();
}
bool APortalV2::IsTravellerBody(AActor* Actor, UPrimitiveComponent* Component)
{
    if (!IsValid(Actor) || !IsValid(Component))
        return false;

    UPortalTravellerComponent* Traveller = Actor->FindComponentByClass<UPortalTravellerComponent>();

    if (!IsValid(Traveller))
        return false;

    if (ACharacter* Character = Cast<ACharacter>(Actor))
        return Component == Character->GetCapsuleComponent();

    if (UPrimitiveComponent* TeleportPrimitive = Traveller->GetTeleportPrimitive())
        return Component == TeleportPrimitive;

    return Component == Cast<UPrimitiveComponent>(Actor->GetRootComponent());
}
bool APortalV2::IsPointInsideOpening(const FVector& WorldPoint)
{
    FVector Relative = WorldPoint - GetPortalLocation();

    float LocalRight = FMath::Abs(FVector::DotProduct(Relative, GetActorRightVector()));
   
    float LocalUp = FMath::Abs(FVector::DotProduct(Relative, GetActorUpVector()));

    return LocalRight <= PortalHalfSize.X + TravellerMargin && LocalUp <= PortalHalfSize.Y + TravellerMargin;
}
void APortalV2::AllPortalBlocksBlockOn(bool On)
{
    FName CollisionProfile = bActive && On ? TEXT("BlockAllDynamic") : TEXT("NoCollision");

    for (UBoxComponent* Box : PortalBlocks) {
        if (!Box)
            continue;
        Box->SetCollisionProfileName(CollisionProfile);
    }
}
ECollisionChannel APortalV2::GetSurfaceCollisionChannel()
{
    if (!IsValid(CurrentPortalSurface))
        return ECC_MAX;

    return CurrentPortalSurface->GetSurfaceCollisionChannel();
}

FMatrix APortalV2::MakeViewMatrix(const FVector& Location, const FQuat& Rotation)
{
    FMatrix Matrix(FPlane(0.f, 0.f, 1.f, 0.f), FPlane(1.f, 0.f, 0.f, 0.f), FPlane(0.f, 1.f, 0.f, 0.f), FPlane(0.f, 0.f, 0.f, 1.f));
    return FTranslationMatrix(-Location) * FInverseRotationMatrix(Rotation.Rotator()) * Matrix;
}

bool APortalV2::SetCameraProjection(FMinimalViewInfo& PlayerView, UTextureRenderTarget2D* Target, USceneCaptureComponent2D* Camera, float OverscanScale)
{
    if (!Camera || !Target)
        return false;

    FMinimalViewInfo View = MakeCaptureView(PlayerView, Target, OverscanScale);

    Camera->FOVAngle = View.FOV;
    Camera->bUseCustomProjectionMatrix = true;
    Camera->CustomProjectionMatrix = View.CalculateProjectionMatrix();

    return true;
}
void APortalV2::CreatePortalRenderTargets()
{


    PortalRecurRenderTarget = nullptr;
    PortalRecurWriteTarget = nullptr;
    InvalidatePortalHistory();

    if (!GetWorld() || !bUseRecursion)
        return;

    PortalRecurRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), RenderTargetWidth* RecursiveRenderScale, RenderTargetHeight* RecursiveRenderScale, ETextureRenderTargetFormat::RTF_RGBA16f);
    if (PortalRecurRenderTarget)
    {
        PortalRecurRenderTarget->ClearColor = FLinearColor::Black;
        PortalRecurRenderTarget->Filter = TF_Bilinear;
        PortalRecurRenderTarget->AddressX = TA_Clamp;
        PortalRecurRenderTarget->AddressY = TA_Clamp;
        PortalRecurRenderTarget->UpdateResourceImmediate(true);
    }
    PortalRecurWriteTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), RenderTargetWidth * RecursiveRenderScale, RenderTargetHeight * RecursiveRenderScale, ETextureRenderTargetFormat::RTF_RGBA16f);

    if (PortalRecurWriteTarget)
    {
        PortalRecurWriteTarget->ClearColor = FLinearColor::Black;
        PortalRecurWriteTarget->Filter = TF_Bilinear;
        PortalRecurWriteTarget->AddressX = TA_Clamp;
        PortalRecurWriteTarget->AddressY = TA_Clamp;
        PortalRecurWriteTarget->UpdateResourceImmediate(true);
    }
}
void APortalV2::SetScreenTexture(UTexture* Texture)
{
    if (!Texture)
        return;

    if (PortalPlaneMaterialInstance)
        PortalPlaneMaterialInstance->SetTextureParameterValue(TEXT("ConnectedRender"), Texture);

    if (PortalNearClipShellMaterialInstance)
        PortalNearClipShellMaterialInstance->SetTextureParameterValue(TEXT("ConnectedRender"), Texture);

}

bool APortalV2::CaptureNormal(FMinimalViewInfo& PlayerView)
{
    if (!PortalCamera || !PortalRenderTarget || !PortalRecurMaterialInstance || !IsValid(LinkPortal))
        return false;


    if (!SetCameraProjection(PlayerView, PortalRenderTarget, PortalCamera, 1.f))
        return false;

    SetDeepOverscanMapping(false);
    FVector CameraLocation = TransformPoint(PlayerView.Location);
    FQuat CameraRotation = TransformRotation(PlayerView.Rotation.Quaternion());

    PortalCamera->SetWorldLocationAndRotation(CameraLocation, CameraRotation);
    PortalCamera->TextureTarget = PortalRenderTarget;

    PortalCamera->ClipPlaneBase = LinkPortal->GetPortalLocation() + LinkPortal->GetActorForwardVector().GetSafeNormal();
    PortalCamera->ClipPlaneNormal = LinkPortal->GetActorForwardVector().GetSafeNormal();


    PortalRecurMaterialInstance->SetScalarParameterValue(TEXT("UseHistory"), 0.f);
    SetRecursionMaterial();


    PortalCamera->CaptureScene();
    RestorePortalRenderState();

    return true;
}
bool APortalV2::CaptureRecursive(FMinimalViewInfo& PlayerView)
{

    if (!PortalCamera  || !PortalRenderTarget || !PortalRecurMaterialInstance || !PortalRecurRenderTarget || !PortalRecurWriteTarget || !IsValid(LinkPortal))
    {
        InvalidatePortalHistory();
        return false;
    }


    FVector V1Location = TransformPoint(PlayerView.Location);
    FQuat V1Rotation = TransformRotation(PlayerView.Rotation.Quaternion());

    FVector V2Location = TransformPoint(V1Location);
    FQuat V2Rotation = TransformRotation(V1Rotation);

    FTransform V1View(V1Rotation, V1Location, FVector::OneVector);
    FTransform V2View(V2Rotation, V2Location, FVector::OneVector);

 
    FPortalClipQuad CurrentHistoryFrontQuad;
    FPortalClipQuad CurrentHistoryBackQuad;

    if (!SetCameraProjection(PlayerView, PortalRecurWriteTarget, PortalCamera, HistoryOverscanScale))
    {
        InvalidatePortalHistory();
        return false;
    }

    bool bCurrentHistoryQuadsValid = GetPortalQuads(V1View, V2View, PortalCamera->CustomProjectionMatrix, CurrentHistoryFrontQuad, CurrentHistoryBackQuad);


    bool bUseHistory = PortalHistory.bValid && bCurrentHistoryQuadsValid;

    if (!SetHistoryMaterial(bUseHistory, CurrentHistoryBackQuad))
        InvalidatePortalHistory();
    
    SetRecursionMaterial();
    SetDeepOverscanMapping(false);

    FEngineShowFlags SavedShowFlags = PortalCamera->ShowFlags;
    ApplyRecursiveShowFlags();

    PortalCamera->SetWorldLocationAndRotation(V2View.GetLocation(), V2View.GetRotation());
    PortalCamera->TextureTarget = PortalRecurWriteTarget;

    PortalCamera->ClipPlaneBase = LinkPortal->GetPortalLocation() + LinkPortal->GetActorForwardVector().GetSafeNormal();
    PortalCamera->ClipPlaneNormal = LinkPortal->GetActorForwardVector().GetSafeNormal();

    PortalCamera->CaptureScene();
    PortalCamera->ShowFlags = SavedShowFlags;

    RestorePortalMaterials();

    if (!SetCameraProjection(PlayerView, PortalRenderTarget, PortalCamera, 1.f))
    {
        RestorePortalRenderState();
        InvalidatePortalHistory();
        return false;
    }
    SetScreenTexture(PortalRecurWriteTarget);
    SetDeepOverscanMapping(true);
    PortalCamera->SetWorldLocationAndRotation(V1View.GetLocation(), V1View.GetRotation());
    PortalCamera->TextureTarget = PortalRenderTarget;


    PortalCamera->CaptureScene();
    
    RestorePortalRenderState();

  
    if (!bCurrentHistoryQuadsValid)
    {
        if (PortalHistory.bValid && HistoryInvalidFrames < HistoryInvalidGraceFrames)
        {
            HistoryInvalidFrames++;
            return true;
        }

        InvalidatePortalHistory();
        return true;
    }
    HistoryInvalidFrames = 0;
    SavePortalHistory(CurrentHistoryFrontQuad);
    return true;
    
}

bool APortalV2::ShouldCapture(FMinimalViewInfo& PlayerView)
{

    if (!bActive || !IsValid(LinkPortal) || !LinkPortal->GetActive())
        return false;

    FVector PortalLocation = GetPortalLocation();
    float Distance = FVector::Distance(PlayerView.Location, PortalLocation);

    if (Distance > MaxCaptureDistance)
        return false;

    if (Distance <= CloseCaptureDistance)
        return true;

    if (FVector::DotProduct(PlayerView.Location - PortalLocation, GetActorForwardVector()) <= 0.f)
        return false;

    return IsPortalVisible(PlayerView);
}


void APortalV2::UpdatePortalRender()
{
    
    APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

    if (!CameraManager)
    {
        InvalidatePortalHistory();
        return;
    }

    FMinimalViewInfo PlayerView = CameraManager->GetCameraCachePOV();
    
    if (!ShouldCapture(PlayerView))
    {
       
        InvalidatePortalHistory();
        return;
    }
   

    if (bUseRecursion && IsRecursivePortalVisible(PlayerView)) {
        if (!CaptureRecursive(PlayerView))
            CaptureNormal(PlayerView);
       
    }
    else
    {
        InvalidatePortalHistory();
        CaptureNormal(PlayerView);
        
    }
}
void APortalV2::SetRecursionMaterial()
{
    if (!PortalRecurMaterialInstance) return;

    if (PortalPlane)
        PortalPlane->SetMaterial(0, PortalRecurMaterialInstance);
    if (PortalNearClipShell)
        PortalNearClipShell->SetMaterial(0, PortalRecurMaterialInstance);
}
void APortalV2::RestorePortalMaterials()
{
    if (PortalPlane && PortalPlaneMaterialInstance)
        PortalPlane->SetMaterial(0, PortalPlaneMaterialInstance);

    if (PortalNearClipShell && PortalNearClipShellMaterialInstance)
        PortalNearClipShell->SetMaterial(0, PortalNearClipShellMaterialInstance);
}
void APortalV2::RestorePortalRenderState()
{
    SetDeepOverscanMapping(false);
    RestorePortalMaterials();
    SetScreenTexture(PortalRenderTarget);

    if (PortalCamera) PortalCamera->TextureTarget = PortalRenderTarget;
}
void APortalV2::ResetPortalRenderHistory()
{
   
    if (PortalRenderTarget)
        UKismetRenderingLibrary::ClearRenderTarget2D(this, PortalRenderTarget, FLinearColor::Black);
   
    if (PortalRecurRenderTarget)
    {
        UKismetRenderingLibrary::ClearRenderTarget2D(this, PortalRecurRenderTarget, FLinearColor::Black);
    }

    if (PortalRecurWriteTarget)
    {
        UKismetRenderingLibrary::ClearRenderTarget2D(this, PortalRecurWriteTarget, FLinearColor::Black);
    }

    InvalidatePortalHistory();
    
    RestorePortalRenderState();
}

bool APortalV2::CalculatePortalClipQuad(FMatrix& ViewProjectionMatrix, FPortalClipQuad& OutQuad)
{
    FVector Center = GetPortalLocation();

    FVector Right = GetActorRightVector().GetSafeNormal() * PortalHalfSize.X;

    FVector Up = GetActorUpVector().GetSafeNormal() * PortalHalfSize.Y;

    FVector WorldLeftUp = Center - Right + Up;

    FVector WorldLeftDown = Center - Right - Up;

    FVector WorldRightUp = Center + Right + Up;

    FVector WorldRightDown = Center + Right - Up;

    auto ProjectPoint = [&ViewProjectionMatrix](const FVector& WorldPoint, FVector4& OutPoint)
        {
            FVector4 Clip = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPoint, 1.f));

            if (!FMath::IsFinite(Clip.X) || !FMath::IsFinite(Clip.Y) || !FMath::IsFinite(Clip.Z) || !FMath::IsFinite(Clip.W))
                return false;

            if (Clip.W <= KINDA_SMALL_NUMBER)
                return false;


            float NdcX = Clip.X / Clip.W;
            float NdcY = Clip.Y / Clip.W;
            float NdcZ = Clip.Z / Clip.W;

            OutPoint.X = NdcX * 0.5f + 0.5f;
            OutPoint.Y = 1.f - (NdcY * 0.5f + 0.5f);
            OutPoint.Z = NdcZ;
            OutPoint.W = Clip.W;

            return FMath::IsFinite(OutPoint.X) && FMath::IsFinite(OutPoint.Y) && FMath::IsFinite(OutPoint.Z) && FMath::IsFinite(OutPoint.W);
        };

    return ProjectPoint(WorldLeftUp, OutQuad.LeftUp) && ProjectPoint(WorldLeftDown, OutQuad.LeftDown) &&
        ProjectPoint(WorldRightUp, OutQuad.RightUp) && ProjectPoint(WorldRightDown, OutQuad.RightDown);
}

bool APortalV2::IsClipQuadValid(FPortalClipQuad& Quad)
{
    FVector4 Points[4] =
    {
        Quad.LeftUp,
        Quad.RightUp,
        Quad.RightDown,
        Quad.LeftDown
    };

    float MinX = TNumericLimits<float>::Max();
    float MinY = TNumericLimits<float>::Max();
    float MaxX = TNumericLimits<float>::Lowest();
    float MaxY = TNumericLimits<float>::Lowest();

    for (FVector4& Point : Points)
    {
        if (!FMath::IsFinite(Point.X) || !FMath::IsFinite(Point.Y) || !FMath::IsFinite(Point.Z) || !FMath::IsFinite(Point.W))
            return false;

        if (Point.W <= KINDA_SMALL_NUMBER)
            return false;
      

        MinX = FMath::Min(MinX, Point.X);
        MinY = FMath::Min(MinY, Point.Y);
        MaxX = FMath::Max(MaxX, Point.X);
        MaxY = FMath::Max(MaxY, Point.Y);

    }

    float Width = MaxX - MinX;
    float Height = MaxY - MinY;
    if (Width <= 0.001f || Height <= 0.001f)
        return false;


    float AreaValue = 0.f;

    for (int32 i = 0; i < 4; i++)
    {
        FVector4& A = Points[i];
        FVector4& B = Points[(i + 1) % 4];

        AreaValue += A.X * B.Y - B.X * A.Y;
    }

    float Area = FMath::Abs(AreaValue) * 0.5f;

    if (Area <= 0.000001f) {
        return false;
    }


    return true;
}
bool APortalV2::SetHistoryMaterial(bool bUseHistory, FPortalClipQuad& CurrentBackQuad)
{

    if (!PortalRecurMaterialInstance)
        return false;

    PortalRecurMaterialInstance->SetScalarParameterValue(TEXT("UseHistory"), 0.f);

    if (!bUseHistory)
        return true;

    if (!PortalRecurRenderTarget)
        return false;


    FPortalHomography HistoryMatrix;

    if (!MakePortalHomography(CurrentBackQuad, PortalHistory.FrontQuad, HistoryMatrix)) {
       
        return false;
    }

    PortalRecurMaterialInstance->SetTextureParameterValue(TEXT("ConnectedRender"), PortalRecurRenderTarget);
    SetHistoryHomography(HistoryMatrix);
    PortalRecurMaterialInstance->SetScalarParameterValue(TEXT("UseHistory"), 1.f);

    return true;
}

void APortalV2::SavePortalHistory(FPortalClipQuad& FrontQuad)
{
    Swap(PortalRecurRenderTarget, PortalRecurWriteTarget);

    PortalHistory.FrontQuad = FrontQuad;
    PortalHistory.bValid = true;
}
void APortalV2::InvalidatePortalHistory()
{
    PortalHistory.Reset();
    HistoryInvalidFrames = 0;
    if (PortalRecurMaterialInstance)
        PortalRecurMaterialInstance->SetScalarParameterValue(TEXT("UseHistory"), 0.f);
}
bool APortalV2::MakeSquareQuad(FPortalClipQuad& Quad, FPortalHomography& OutMatrix)
{
    FVector2D LeftUP(Quad.LeftUp.X, Quad.LeftUp.Y);
    FVector2D RightUp(Quad.RightUp.X, Quad.RightUp.Y);
    FVector2D RightDown(Quad.RightDown.X, Quad.RightDown.Y);
    FVector2D LeftDown(Quad.LeftDown.X, Quad.LeftDown.Y);

    float X1 = RightUp.X - RightDown.X;
    float X2 = LeftDown.X - RightDown.X;
    float X3 = LeftUP.X - RightUp.X + RightDown.X - LeftDown.X;

    float Y1 = RightUp.Y - RightDown.Y;
    float Y2 = LeftDown.Y - RightDown.Y;
    float Y3 = LeftUP.Y - RightUp.Y + RightDown.Y - LeftDown.Y;


    if (FMath::IsNearlyZero(X3) && FMath::IsNearlyZero(Y3)) {
        OutMatrix.M[0][0] = RightUp.X - LeftUP.X; OutMatrix.M[0][1] = LeftDown.X - LeftUP.X; OutMatrix.M[0][2] = LeftUP.X;
        OutMatrix.M[1][0] = RightUp.Y - LeftUP.Y; OutMatrix.M[1][1] = LeftDown.Y - LeftUP.Y; OutMatrix.M[1][2] = LeftUP.Y;
        OutMatrix.M[2][0] = 0; OutMatrix.M[2][1] = 0; OutMatrix.M[2][2] = 1;
        return true;
    }

    float Determinant = X1 * Y2 - X2 * Y1;

    if (FMath::Abs(Determinant) <= KINDA_SMALL_NUMBER)
        return false;

    float G = (X3 * Y2 - X2 * Y3) / Determinant;
    float H = (X1 * Y3 - X3 * Y1) / Determinant;

    OutMatrix.M[0][0] = RightUp.X - LeftUP.X + G * RightUp.X; OutMatrix.M[0][1] = LeftDown.X - LeftUP.X + H * LeftDown.X; OutMatrix.M[0][2] = LeftUP.X;
    OutMatrix.M[1][0] = RightUp.Y - LeftUP.Y + G * RightUp.Y; OutMatrix.M[1][1] = LeftDown.Y - LeftUP.Y + H * LeftDown.Y; OutMatrix.M[1][2] = LeftUP.Y;
    OutMatrix.M[2][0] = G; OutMatrix.M[2][1] = H; OutMatrix.M[2][2] = 1;

    return true;
}
bool APortalV2::InverseHomography(FPortalHomography& Matrix, FPortalHomography& OutInverse)
{
    float A = Matrix.M[0][0], B = Matrix.M[0][1], C = Matrix.M[0][2];
    float D = Matrix.M[1][0], E = Matrix.M[1][1], F = Matrix.M[1][2];
    float G = Matrix.M[2][0], H = Matrix.M[2][1], I = Matrix.M[2][2];

    float Determinant = A * (E * I - F * H) - B * (D * I - F * G) + C * (D * H - E * G);

    if (FMath::Abs(Determinant) <= KINDA_SMALL_NUMBER)
        return false;

    float InvDet = 1.f / Determinant;

    OutInverse.M[0][0] = (E * I - F * H) * InvDet;
    OutInverse.M[0][1] = (C * H - B * I) * InvDet;
    OutInverse.M[0][2] = (B * F - C * E) * InvDet;

    OutInverse.M[1][0] = (F * G - D * I) * InvDet;
    OutInverse.M[1][1] = (A * I - C * G) * InvDet;
    OutInverse.M[1][2] = (C * D - A * F) * InvDet;

    OutInverse.M[2][0] = (D * H - E * G) * InvDet;
    OutInverse.M[2][1] = (B * G - A * H) * InvDet;
    OutInverse.M[2][2] = (A * E - B * D) * InvDet;

    return true;
}
FPortalHomography APortalV2::MultiplyHomography(FPortalHomography& A, FPortalHomography& B)
{
    FPortalHomography Result;

    for (int32 i = 0; i < 3; i++) {
        for (int32 l = 0; l < 3; l++) {
            Result.M[i][l] = 0.f;

            for (int j = 0; j < 3; j++)
                Result.M[i][l] += A.M[i][j] * B.M[j][l];
        }
    }
    return Result;
}
bool APortalV2::MakePortalHomography(FPortalClipQuad& CurrentQuad, FPortalClipQuad& HistoryQuad, FPortalHomography& OutMatrix)
{
    FPortalHomography CurrentMatrix;
    FPortalHomography HistoryMatrix;
    FPortalHomography InverseCurrent;

    if (!MakeSquareQuad(CurrentQuad, CurrentMatrix))
        return false;
    //Æ÷Å» ³»ºÎ 0~1 ÁÂÇ¥¸¦ È­¸é»óÀÇ Æ÷Å» ScreenUV ÁÂÇ¥·Î º¸³»´Â Matrix »ý¼º ÇÔ¼ö´Ù.
    if (!MakeSquareQuad(HistoryQuad, HistoryMatrix))
        return false;

    if (!InverseHomography(CurrentMatrix, InverseCurrent))
        return false;
    //Æ÷Å» 
    OutMatrix = MultiplyHomography(HistoryMatrix, InverseCurrent);

    return true;
}
void APortalV2::SetHistoryHomography(FPortalHomography& Matrix)
{
    if (!PortalRecurMaterialInstance)
        return;

    PortalRecurMaterialInstance->SetVectorParameterValue(TEXT("History0"), FLinearColor(Matrix.M[0][0], Matrix.M[0][1], Matrix.M[0][2], 0.f));
    PortalRecurMaterialInstance->SetVectorParameterValue(TEXT("History1"), FLinearColor(Matrix.M[1][0], Matrix.M[1][1], Matrix.M[1][2], 0.f));
    PortalRecurMaterialInstance->SetVectorParameterValue(TEXT("History2"), FLinearColor(Matrix.M[2][0], Matrix.M[2][1], Matrix.M[2][2], 0.f));
}

FMinimalViewInfo APortalV2::MakeCaptureView(FMinimalViewInfo& PlayerView, UTextureRenderTarget2D* Target, float OverscanScale)
{
    FMinimalViewInfo View = PlayerView;

    if (Target)
        View.AspectRatio = static_cast<float>(Target->SizeX) / static_cast<float>(FMath::Max(1, Target->SizeY));

    View.bConstrainAspectRatio = false;

    float SafeScale = FMath::Max(1.f, OverscanScale);
    float BaseFOV = FMath::Clamp(View.FOV, 5.f, 170.f);
    float HalfFOV = FMath::DegreesToRadians(BaseFOV * 0.5f);
    float OverscanFOV = FMath::RadiansToDegrees(2.f * FMath::Atan(FMath::Tan(HalfFOV) * SafeScale));

    View.FOV = FMath::Min(OverscanFOV, 170.f);

    return View;
}
bool APortalV2::GetPortalQuads(const FTransform& FrontView, const FTransform& BackView, const FMatrix& ProjectionMatrix, FPortalClipQuad& OutFrontQuad, FPortalClipQuad& OutBackQuad)
{
    FMatrix BackViewProjection = MakeViewMatrix(BackView.GetLocation(), BackView.GetRotation()) * ProjectionMatrix;
    FMatrix FrontViewProjection = MakeViewMatrix(FrontView.GetLocation(), FrontView.GetRotation()) * ProjectionMatrix;

    if (!CalculatePortalClipQuad(BackViewProjection, OutBackQuad))
        return false;

    if (!CalculatePortalClipQuad(FrontViewProjection, OutFrontQuad))
        return false;

    return IsClipQuadValid(OutBackQuad) && IsClipQuadValid(OutFrontQuad);
}


void APortalV2::SetDeepOverscanMapping(bool bEnable)
{
    float UseMapping = bEnable ? 1.f : 0.f;

    if (PortalPlaneMaterialInstance)
    {
        PortalPlaneMaterialInstance->SetScalarParameterValue(TEXT("UseDeepOverscanMapping"), UseMapping);
    }

    if (PortalNearClipShellMaterialInstance)
    {
    
        PortalNearClipShellMaterialInstance->SetScalarParameterValue(TEXT("UseDeepOverscanMapping"), UseMapping);
    }
}

bool APortalV2::IsPortalVisibleView(const FVector& Location,const FQuat& Rotation,const FMatrix& Matrix)
{
   
    FMatrix ViewProjection = MakeViewMatrix(Location, Rotation) * Matrix;
    FVector Center = GetPortalLocation();
    FVector Right = GetActorRightVector().GetSafeNormal() * PortalHalfSize.X;
    FVector Up = GetActorUpVector().GetSafeNormal() * PortalHalfSize.Y;

    FVector Corners[4] = {
         Center - Right + Up,
        Center - Right - Up,
        Center + Right + Up,
        Center + Right - Up
    };

    FVector4 ClipPoints[4];

    int32 FrontCount = 0;

    for (int32 i = 0; i < 4; i++) {
        ClipPoints[i] = ViewProjection.TransformFVector4(FVector4(Corners[i], 1.f));

        if (!FMath::IsFinite(ClipPoints[i].X) || !FMath::IsFinite(ClipPoints[i].Y) || !FMath::IsFinite(ClipPoints[i].W))
            return true;

        if (ClipPoints[i].W > KINDA_SMALL_NUMBER)
            FrontCount++;
    }

    if (FrontCount == 0)
        return false;

    if (FrontCount < 4)
        return true;

    bool bLeft = true;
    bool bRight = true;
    bool bUp = true;
    bool bDown = true;

    for (FVector4& Clip : ClipPoints) {
        float X = Clip.X / Clip.W;
        float Y = Clip.Y / Clip.W;
        bLeft &= X < -1.f;
        bRight &= X > 1.f;
        bUp &= Y > 1.f;
        bDown &= Y < -1.f;
    }
    return !(bLeft || bRight || bUp || bDown);
   
}

bool APortalV2::IsPortalVisible(FMinimalViewInfo& PlayerView)
{

    FMatrix ProjectionMatrix = PlayerView.CalculateProjectionMatrix();

    return IsPortalVisibleView(PlayerView.Location, PlayerView.Rotation.Quaternion(), ProjectionMatrix);

}

bool APortalV2::IsRecursivePortalVisible(FMinimalViewInfo& PlayerView)
{
    if (!PortalRenderTarget || !IsValid(LinkPortal))
        return false;

    FVector Location = TransformPoint(PlayerView.Location);
    FQuat Rotation = TransformRotation(PlayerView.Rotation.Quaternion());
    FMinimalViewInfo View = MakeCaptureView(PlayerView, PortalRenderTarget, 1.f);

    return IsPortalVisibleView(Location, Rotation, View.CalculateProjectionMatrix());
   
}

void APortalV2::ApplyRecursiveShowFlags()
{
    if (!PortalCamera)
        return;

    PortalCamera->ShowFlags.SetDepthOfField(false);
    PortalCamera->ShowFlags.SetLensFlares(false);
    PortalCamera->ShowFlags.SetGrain(false);
    PortalCamera->ShowFlags.SetVignette(false);
    PortalCamera->ShowFlags.SetCameraImperfections(false);
    PortalCamera->ShowFlags.SetSceneColorFringe(false);

    PortalCamera->ShowFlags.SetScreenSpaceReflections(false);
}

bool APortalV2::TeleportObjects(AActor* Actor, UPortalTravellerComponent* Traveller)
{
    if (!IsValid(Actor) || !Traveller || !IsValid(LinkPortal))
        return false;


    FVector OldLocation;
    FQuat OldRotation;
    FVector OldLinearVelocity;
    FVector OldAngularVelocity;

    if (!Traveller->GetPhysicsData(OldLocation, OldRotation, OldLinearVelocity, OldAngularVelocity))
        return false;

    FVector NewLocation = TransformPoint(OldLocation);
    FQuat NewRotation = TransformRotation(OldRotation);
  
    bool bGrab = Traveller->GetGrabObject();

    FVector NewLinearVelocity = TransformVector(OldLinearVelocity);
    FVector NewAngularVelocity = TransformVector(OldAngularVelocity);


    FVector ExitDirection = bGrab ? LinkPortal->GetActorForwardVector().GetSafeNormal() :
        (!NewLinearVelocity.IsNearlyZero() ? NewLinearVelocity.GetSafeNormal() : LinkPortal->GetActorForwardVector().GetSafeNormal());

    NewLocation += ExitDirection * ExitOffset;


    return Traveller->SetPhysicsData(NewLocation, NewRotation, NewLinearVelocity, NewAngularVelocity);
}




