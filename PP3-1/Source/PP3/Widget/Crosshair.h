// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Crosshair.generated.h"

/**
 * 
 */
UCLASS()
class PP3_API UCrosshair : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UImage* m_CrosshairImage;
	
	
	
	FVector2D HUDSize;

	UCanvasPanelSlot* m_PanelSlot;
protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	
};
