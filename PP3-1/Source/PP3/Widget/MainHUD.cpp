// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"

void UMainHUD::NativeConstruct()
{
	Super::NativeConstruct();
	
	m_Crosshair = Cast<UCrosshair>(GetWidgetFromName(TEXT("UI_Crosshair")));
	m_GunMode = Cast<UGunMode>(GetWidgetFromName(TEXT("UI_GunMode")));
}
void UMainHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
}
