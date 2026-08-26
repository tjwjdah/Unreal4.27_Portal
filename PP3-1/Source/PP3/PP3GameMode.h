// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameInfo.h"
#include "Widget/MainHUD.h"
#include "GameFramework/GameModeBase.h"
#include "PP3GameMode.generated.h"

UCLASS(minimalapi)
class APP3GameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UMainHUD>	m_MainHUDClass;
	UMainHUD* m_MainHUD;
public:
	APP3GameMode();

public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);
	virtual void BeginPlay() override;


public:
	UMainHUD* GetMainHUD()
	{
		return m_MainHUD;
	}

};



