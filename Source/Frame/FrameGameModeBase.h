// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FrameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FRAME_API AFrameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	virtual void PawnKilled(APawn* PawnKilled);
	
};
