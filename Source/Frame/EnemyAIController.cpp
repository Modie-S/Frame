// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Enemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "FrameCharacter.h"

AEnemyAIController::AEnemyAIController()
{
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
    check(BlackboardComponent);

    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
    check(BehaviorTreeComponent);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (InPawn == nullptr) return;

    AEnemy* Enemy = Cast<AEnemy>(InPawn);
    if (Enemy)
    {
        if (Enemy->GetBehaviorTree())
        {
            BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
        }
    }
}

bool AEnemyAIController::IsDead() const
{
    AFrameCharacter* ControllingCharacter = Cast<AFrameCharacter>(GetPawn());
    if (ControllingCharacter != nullptr)
    {
        ControllingCharacter->Die();
    }

    return true;
}