// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

#include "GunMode.generated.h"

/**
 * 
 */
UCLASS()
class PP3_API UGunMode : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UTextBlock* m_Mode;
	


protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	void SetMode(EGunMode _Mode);

};
