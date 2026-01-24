#include "CoinItem.h"
#include "SpartaGameState.h"

ACoinItem::ACoinItem()
{
    PointValue = 0;
    ItemType = "DefaultCoin";
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


