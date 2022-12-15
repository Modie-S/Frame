// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrameGameModeBase.h"
#include "KillAllEnemiesGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FRAME_API AKillAllEnemiesGameMode : public AFrameGameModeBase
{
	GENERATED_BODY()
	
public:

	virtual void PawnKilled(APawn* PawnKilled) override;

private:

	void EndGame(bool bIsPlayerWinner);
};
