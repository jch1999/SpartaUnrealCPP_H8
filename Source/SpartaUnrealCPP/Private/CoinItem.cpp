#include "CoinItem.h"
#include "SpartaGameState.h"

ACoinItem::ACoinItem()
{
    PointValue = 0;
    ItemType = "DefaultCoin";
    RotateSpeed = 180.0f;
    PrimaryActorTick.bCanEverTick = true;
}

void ACoinItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);
    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (UWorld* World = GetWorld())
        {
            if (World->GetGameState())
            {
                if (ASpartaGameState* GameState = Cast<ASpartaGameState>(World->GetGameState()))
                {
                    GameState->AddScore(PointValue);
                    GameState->OnCoinCollected();
                }
            }
        }
        DestroyItem();
    }
}

void ACoinItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AddActorLocalRotation(FRotator(0, RotateSpeed * DeltaTime, 0));
}


