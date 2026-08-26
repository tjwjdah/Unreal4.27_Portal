// Fill out your copyright notice in the Description page of Project Settings.


#include "GunMode.h"

void UGunMode::NativeConstruct()
{
	Super::NativeConstruct();

	m_Mode = Cast<UTextBlock>(GetWidgetFromName(TEXT("ModeText")));

}
void UGunMode::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UGunMode::SetMode(EGunMode _Mode)
{
	if (_Mode == EGunMode::Portal)
	{
		m_Mode->SetText(FText::FromString(TEXT("Portal")));
		m_Mode->SetColorAndOpacity(FSlateColor(FLinearColor(0.0f, 1.0f, 1.0f, 1.0f)));
	}
	else {
		m_Mode->SetText(FText::FromString(TEXT("Grab")));
		m_Mode->SetColorAndOpacity(FSlateColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)));
	}
}
