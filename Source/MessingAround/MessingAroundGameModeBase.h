// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MessingAroundGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MESSINGAROUND_API AMessingAroundGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void StartPlay() override;
};
