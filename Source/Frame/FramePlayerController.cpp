// Fill out your copyright notice in the Description page of Project Settings.


#include "FramePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

AFramePlayerController::AFramePlayerController()
{

}

void AFramePlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
    Super::GameHasEnded(EndGameFocus, bIsWinner);

    if (bIsWinner)
    {
        UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
            if (WinScreen != nullptr)
            {
                WinScreen->AddToViewport();
            }
    }
    else
    {
        UUserWidget* GameOverScreen = CreateWidget(this, GameOverClass);
            if (GameOverScreen != nullptr)
            {
                GameOverScreen->AddToViewport();
            }
    }
   
    GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}

void AFramePlayerController::BeginPlay()
{
    Super::BeginPlay();

    //Check HUD Overlay class TSubclassOf variable
    if (HUDOverlayClass)
    {
        HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
        if (HUDOverlay)
        {
            HUDOverlay->AddToViewport();
            HUDOverlay->SetVisibility(ESlateVisibility::Visible);
        }
    }
}