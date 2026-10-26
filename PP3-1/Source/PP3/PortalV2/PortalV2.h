// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "PortalV2.generated.h"

struct FPortalClipQuad
{
    FVector4 LeftUp = FVector4(0.f, 0.f, 0.f, 0.f);
    FVector4 LeftDown = FVector4(0.f, 0.f, 0.f, 0.f);
    FVector4 RightUp = FVector4(0.f, 0.f, 0.f, 0.f);
    FVector4 RightDown = FVector4(0.f, 0.f, 0.f, 0.f);
};
struct FPortalHomography
{
    float M[3][3] =
    {
        { 1.f, 0.f, 0.f },
        { 0.f, 1.f, 0.f },
        { 0.f, 0.f, 1.f }
    };
};
struct FPortalHistoryState
{
    FPortalClipQuad FrontQuad;
    bool bValid = false;

    void Reset()
    {
        FrontQuad = FPortalClipQuad();
        bValid = false;
    }
};

class APortalV2;

USTRUCT()
struct FPortalRenderTickFunction : public FTickFunction
{
    GENERATED_BODY()

    APortalV2* Target = nullptr;

    virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
    virtual FString DiagnosticMessage() override;
};

template<>
struct TStructOpsTypeTraits<FPortalRenderTickFunction> : public TStructOpsTypeTraitsBase2<FPortalRenderTickFunction>
{
    enum
    {
        WithCopy = false
    };
};

class UPortalTravellerComponent;
UCLASS()
class PP3_API APortalV2 : public AActor
{
    GENERATED_BODY()

    public:
    // Sets default values for this actor's properties
    APortalV2();
private:
    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootSceneComponent;

    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootVisual;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* PortalPlane;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* PortalInner;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* PortalNearClipShell;

    UPROPERTY(VisibleAnywhere)
    UDecalComponent* PortalDecal;

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* PortalEnterTrigger;

    UPROPERTY(VisibleAnywhere)
    USceneCaptureComponent2D* PortalCamera;


    UPROPERTY(VisibleAnywhere)
    UArrowComponent* DirectionArrow;

    UPROPERTY(EditDefaultsOnly)
    UMaterialInterface* PortalPlaneMaterial;

    UPROPERTY(EditDefaultsOnly)
    UMaterialInterface* PortalInnerMaterial;

    UPROPERTY(EditDefaultsOnly)
    UMaterialInterface* PortalDecalMaterial;

    UPROPERTY(VisibleDefaultsOnly)
    int32 RenderTargetWidth;

    UPROPERTY(VisibleDefaultsOnly)
    int32 RenderTargetHeight;

    UPROPERTY(EditAnywhere)
    APortalV2* LinkPortal;

    UPROPERTY(Transient)
    UTextureRenderTarget2D* PortalRenderTarget;

    UPROPERTY(Transient)
    UMaterialInstanceDynamic* PortalPlaneMaterialInstance;

    UPROPERTY(Transient)
    UMaterialInstanceDynamic* PortalInnerMaterialInstance;

    UPROPERTY(Transient)
    UMaterialInstanceDynamic* PortalDecalMaterialInstance;
    UPROPERTY(Transient)
    UMaterialInstanceDynamic* PortalNearClipShellMaterialInstance;


    UPROPERTY(VisibleDefaultsOnly)
    bool bActive;


    UPROPERTY(EditAnywhere)
    FLinearColor PortalColor;

    UPROPERTY(EditAnywhere)
    FLinearColor InnerColor;

    UPROPERTY(VisibleDefaultsOnly)
    TSet<AActor*> OverlappingActors;


    UPROPERTY(EditAnywhere)
    float ExitOffset;

    UPROPERTY(EditAnywhere)
    float PortalHolePadding;

    UPROPERTY(EditAnywhere)
    FVector2D PortalHalfSize;

    UPROPERTY(VisibleDefaultsOnly)
    class UPortalSurfaceComponent* CurrentPortalSurface;

    UPROPERTY(EditAnywhere)
    EPortalType PortalType;

    UPROPERTY(EditAnywhere)
    float TravellerMargin;


    UPROPERTY(EditDefaultsOnly)
    TArray<UBoxComponent*> PortalBlocks;


    FVector CurrentSurfacePoint;


    UPROPERTY(EditDefaultsOnly)
    bool bUseRecursion = true;




    UPROPERTY(EditDefaultsOnly)
    float MaxCaptureDistance = 8000.f;

    UPROPERTY(EditDefaultsOnly)
    float CloseCaptureDistance = 200.f;

    UPROPERTY(EditDefaultsOnly)
    UMaterialInterface* PortalRecurMaterial;

    UPROPERTY(Transient)
    UMaterialInstanceDynamic* PortalRecurMaterialInstance = nullptr;

    UPROPERTY(Transient)
    UTextureRenderTarget2D* PortalRecurRenderTarget = nullptr;

    UPROPERTY(Transient)
    UTextureRenderTarget2D* PortalRecurWriteTarget = nullptr;

    FPortalHistoryState PortalHistory;

    UPROPERTY(EditAnywhere)
    float HistoryOverscanScale = 1.15f;

    FPortalRenderTickFunction PortalRenderTick;


    int32 HistoryInvalidFrames = 0;

    UPROPERTY(EditDefaultsOnly)
    int32 HistoryInvalidGraceFrames = 1;


    UPROPERTY(EditDefaultsOnly)
    float RecursiveRenderScale = 0.75f;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void RegisterActorTickFunctions(bool bRegister) override;
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    void InitializePortalRenderTarget();
    void InitializePortalMaterials();



    void ActivatePortal();
    void DeActivatePortal();

    void SetLinkPortal(APortalV2* InLinkPortal);
    APortalV2* GetLinkPortal() { return LinkPortal; }

    bool GetActive() { return bActive; }

    FVector GetPortalLocation();

    USceneCaptureComponent2D* GetPortalCamera() { return PortalCamera; };

    UFUNCTION()
    void OnPortalTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPortalTriggerEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


    void AddTraveller(AActor* Actor);
    void RemoveTraveller(AActor* Actor);


    FVector TransformPoint(const FVector& WorldPoint);

    FVector TransformVector(const FVector& WorldVector);

    FQuat TransformRotation(const FQuat& WorldRotation);


    bool PortalTeleport(AActor* Actor);

    void PlaceSurface(const FVector& NewLocation, const FRotator& NewRotation, const FVector& SurfacePoint, UPortalSurfaceComponent* NewSurface);

    void AddSurfaceHole();
    void ClearSurfaceHole();

    void HiddenComponentsUpdate();

    FVector2D GetPortalHalfSize() { return PortalHalfSize; }
    bool FindCrossPoint(const FVector& PreviousLocation, const FVector& CurrentLocation, FVector& OutPoint);

    bool TeleportObjects(AActor* Actor, UPortalTravellerComponent* Traveller);

    bool TeleportPlayer(AActor* Actor, UPortalTravellerComponent* Traveller);

    void ClearTravellers();


    bool IsTravellerBody(AActor* Actor, UPrimitiveComponent* Component);

    UStaticMeshComponent* GetPortalPlane() { return PortalPlane; }

    bool IsPointInsideOpening(const FVector& WorldPoint);

    void AllPortalBlocksBlockOn(bool On);

    ECollisionChannel GetSurfaceCollisionChannel();


    FMatrix MakeViewMatrix(const FVector& Location, const FQuat& Rotation);
    bool SetCameraProjection(FMinimalViewInfo& PlayerView, UTextureRenderTarget2D* Target, USceneCaptureComponent2D* Camera, float OverscanScale);

    void CreatePortalRenderTargets();
    void SetScreenTexture(UTexture* Texture);

    bool CaptureNormal(FMinimalViewInfo& PlayerView);
    bool CaptureRecursive(FMinimalViewInfo& PlayerView);

    bool ShouldCapture(FMinimalViewInfo& PlayerView);

    void UpdatePortalRender();


    void SetRecursionMaterial();
    void RestorePortalMaterials();
    void RestorePortalRenderState();
    void ResetPortalRenderHistory();


    bool CalculatePortalClipQuad(FMatrix& ViewProjectionMatrix, FPortalClipQuad& OutQuad);

    bool IsClipQuadValid(FPortalClipQuad& Quad);

    bool SetHistoryMaterial(bool bUseHistory, FPortalClipQuad& CurrentBackQuad);

    void SavePortalHistory(FPortalClipQuad& FrontQuad);

    void InvalidatePortalHistory();

    bool MakeSquareQuad(FPortalClipQuad& Quad, FPortalHomography& OutMatrix);
    bool InverseHomography(FPortalHomography& Matrix, FPortalHomography& OutInverse);
    FPortalHomography MultiplyHomography(FPortalHomography& A, FPortalHomography& B);
    bool MakePortalHomography(FPortalClipQuad& CurrentQuad, FPortalClipQuad& HistoryQuad, FPortalHomography& OutMatrix);
    void SetHistoryHomography(FPortalHomography& Matrix);


    FMinimalViewInfo MakeCaptureView(FMinimalViewInfo& PlayerView, UTextureRenderTarget2D* Target, float OverscanScale);

    bool GetPortalQuads(const FTransform& FrontView, const FTransform& BackView, const FMatrix& ProjectionMatrix, FPortalClipQuad& OutFrontQuad, FPortalClipQuad& OutBackQuad);

    void SetDeepOverscanMapping(bool bEnable);
    bool IsPortalVisibleView(const FVector& Location, const FQuat& Rotation, const FMatrix& Matrix);
    bool IsPortalVisible(FMinimalViewInfo& PlayerView);
    bool IsRecursivePortalVisible(FMinimalViewInfo& PlayerView);

    void ApplyRecursiveShowFlags();

    bool tVisible = false;
    void TestVisible();

    void PreparePlayerCapture(bool bFinalCapture);

    void UpdateRenderTargetSize();
};
