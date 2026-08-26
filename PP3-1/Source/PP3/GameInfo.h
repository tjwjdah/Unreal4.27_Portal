// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"

#include "Components/DecalComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SplineComponent.h"

#include "Camera/CameraActor.h"

#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "Engine/StaticMeshActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"

#include "GameFramework/Controller.h"


DECLARE_LOG_CATEGORY_EXTERN(UEKR2, Log, All);
#define	LOG_CALLINFO	(FString(__FUNCTION__) + TEXT("{") + FString::FromInt(__LINE__) + TEXT("}"))
#define	LOG(Format, ...)	UE_LOG(UEKR2, Warning, TEXT("%s : %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define	LOGSTRING(Str)		UE_LOG(UEKR2, Warning, TEXT("%s : %s"), *LOG_CALLINFO, *Str)

#define GET( type ,param) type Get##param () {return m_##param;}
#define SET( type ,param) void Set##param (type _in) {m_##param = _in;} 

#define GET_SET(type, param) GET(type,param) \
							 SET(type,param)


void PrintViewport(float Time, const FColor& Color, const FString& Text);

UENUM(BlueprintType)
enum class EDirection : uint8
{
	Up,
	Down,
	Left,
	Right
};

UENUM(BlueprintType)
enum class EPortalType : uint8
{
	PIN,
	POUT
};

UENUM(BlueprintType)
enum class EGunMode : uint8
{
	Portal,
	Grab
};
extern int32 SpawnNum;