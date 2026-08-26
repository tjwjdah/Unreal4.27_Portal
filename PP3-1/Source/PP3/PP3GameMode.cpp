// Copyright Epic Games, Inc. All Rights Reserved.

#include "PP3GameMode.h"
#include "Character/PP3Character.h"
#include "UObject/ConstructorHelpers.h"

APP3GameMode::APP3GameMode()
{
	// set default pawn class to our Blueprinted character
	
	static ConstructorHelpers::FClassFinder<UMainHUD>	MainHUDClass(TEXT("WidgetBlueprint'/Game/Widget/BP_MainHUD.BP_MainHUD_C'"));

	if (MainHUDClass.Succeeded())
		m_MainHUDClass = MainHUDClass.Class;
		
	static ConstructorHelpers::FClassFinder<APP3Character>	MainCharacter(TEXT("Blueprint'/Game/Character/m_PortalCharacter.m_PortalCharacter_C'"));

	if (MainCharacter.Succeeded())
		DefaultPawnClass = MainCharacter.Class;
}
void APP3GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

}

void APP3GameMode::BeginPlay()
{

	if (IsValid(m_MainHUDClass))
	{
		m_MainHUD = Cast<UMainHUD>(CreateWidget(GetWorld(),
			m_MainHUDClass));

		if (m_MainHUD)
		{
			m_MainHUD->AddToViewport();
		}
	}

}