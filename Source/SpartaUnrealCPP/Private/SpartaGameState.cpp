#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpartaGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

ASpartaGameState::ASpartaGameState()
{
    Score = 0;
    SpawnedCoinCnt = 0;
    CollectedCoinCnt = 0;
    ExplodeCnt = 0;
    MaxExplodeCnt = 3;
    LevelDuration = 45.0f;
    CurrentLevelIndex = 0;
    MaxLevel = 3;
}

void ASpartaGameState::BeginPlay()
{
    Super::BeginPlay();

    StartLevel();
    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &ASpartaGameState::UpdateHUD,
        0.1f,
        true
    );
}

int32 ASpartaGameState::GetScore() const
{
    return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
    UpdateHUD();
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (USpartaGameInstance* SpartaGameInstance =
            Cast<USpartaGameInstance>(GameInstance))
        {
            SpartaGameInstance->AddToScore(Amount);
        }
    }
    /*
    Score += Amount;
    UE_LOG(LogTemp, Warning, TEXT("Score: %d"), Score);*/
}

void ASpartaGameState::IncreaseExplodeCnt()
{
    ExplodeCnt++;
    if (ExplodeCnt > MaxExplodeCnt)
    {
        EndLevel();
    }
}

void ASpartaGameState::OnGameOver()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->SetPause(true);
            SpartaPlayerController->ShowMainMenu(true);
        }
    }
}

void ASpartaGameState::StartLevel()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->ShowGameHUD();
        }
    }
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (USpartaGameInstance* SpartaGameInstance =
            Cast<USpartaGameInstance>(GameInstance))
        {
            CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
        }
    }

    SpawnedCoinCnt = 0;
    CollectedCoinCnt = 0;

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    const int32 ItemToSpawn = 40;

    for (int32 i = 0; i < ItemToSpawn; i++)
    {
        if (FoundVolumes.Num() > 0)
        {
            ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
            if (SpawnVolume)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCnt++;
                }
            }
        }
    }

    GetWorldTimerManager().SetTimer(
        LevelTimerHandle,
        this,
        &ASpartaGameState::OnLevelTimeUP,
        LevelDuration,
        false
    );
}

void ASpartaGameState::OnLevelTimeUP()
{
    // EndLevel();
    OnGameOver();
}

void ASpartaGameState::EndLevel()
{
    GetWorldTimerManager().ClearTimer(LevelTimerHandle);
    CurrentLevelIndex++;

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (USpartaGameInstance* SpartaGameInstance =
            Cast<USpartaGameInstance>(GameInstance))
        {
            // AddScore(Score);
            SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
        }
    }
    if (CurrentLevelIndex >= MaxLevel)
    {
        OnGameOver();
    }

    if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
    {
        UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
    }
    else
    {
        OnGameOver();
    }
}

void ASpartaGameState::OnCoinCollected()
{
    CollectedCoinCnt++;
    UE_LOG(LogTemp, Warning, TEXT("Coin Collceted: %d / %d"),
        CollectedCoinCnt,
        SpawnedCoinCnt);

    if (SpawnedCoinCnt > 0 && CollectedCoinCnt >= SpawnedCoinCnt)
    {
        EndLevel();
    }
}

void ASpartaGameState::UpdateHUD()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController =
            Cast<ASpartaPlayerController>(PlayerController))
        {
            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
                    TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
                }
            }
            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
                {
                    if (UGameInstance* GameInstance = GetGameInstance())
                    {
                        if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
                        {

                            ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
                        }
                    }
                }
            }
            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
                {
                    LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
                }
            }
        }
    }

}
