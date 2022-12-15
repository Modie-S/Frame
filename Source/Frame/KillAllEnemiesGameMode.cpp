// Fill out your copyright notice in the Description page of Project Settings.


#include "KillAllEnemiesGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/Controller.h"
#include "EnemyAIController.h"


void AKillAllEnemiesGameMode::PawnKilled(APawn* PawnKilled)
{
    Super::PawnKilled(PawnKilled);

    APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
    if (PlayerController != nullptr)
    {
       EndGame(false);
    }

    for (AEnemyAIController* Controller : TActorRange<AEnemyAIController>(GetWorld()))
    {
        if (!Controller->IsDead())
        {
            return;
        }
    }

    EndGame(true);
}

void AKillAllEnemiesGameMode::EndGame(bool bIsPlayerWinner)
{
    for (AController* Controller : TActorRange<AController>(GetWorld()))
    {
        bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner; // If player is controller and player is winner, both equal therefore player wins. Same for AI.
        Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
    }
}
