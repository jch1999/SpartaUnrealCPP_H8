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
    CurrentLevelIndex = 0;
    MaxLevel = 3;
    CurrentWaveIndex = 0;
    MaxWave = 3;
    WaveDurationArray = { 60.0f, 45.0f, 30.0f };
    WaveWaitDuration = 5.0f;
    bIsWaveWating = true;
    ItemMaxSpawnCntPerWave = { 20, 30, 40 };
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

    GetWorldTimerManager().SetTimer(
        WaveTimerHandle,
        this,
        &ASpartaGameState::StartWave,
        WaveWaitDuration,
        false
    );
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

void ASpartaGameState::StartWave()
{
    bIsWaveWating = false;
    SpawnedCoinCnt = 0;
    CollectedCoinCnt = 0;

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);


    for (int32 i = 0; i < ItemMaxSpawnCntPerWave[CurrentWaveIndex]; i++)
    {
        if (FoundVolumes.Num() > 0)
        {
            ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
            if (SpawnVolume)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem(CurrentWaveIndex);
                if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCnt++;
                }
            }
        }
    }

    GetWorldTimerManager().SetTimer(
        WaveTimerHandle,
        this,
        &ASpartaGameState::OnWaveTimeUP,
        WaveDurationArray[CurrentWaveIndex],
        false
    );
}

void ASpartaGameState::OnWaveTimeUP()
{
    // Todo 코인을 다 모으지 못했다면 게임 종료
    if (SpawnedCoinCnt != CollectedCoinCnt)
    {
        OnGameOver();
    }
    else
    {
        EndWave();
    }
}

void ASpartaGameState::EndWave()
{
    //Todo. 아이템 전부 제거
    TArray<AActor*> FoundItems;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), FoundItems);
    for (auto Item : FoundItems)
    {
        Item->Destroy();
    }

    // Todo. 일정 시간 동안 대기 후 다음 레벨 시작
    GetWorldTimerManager().ClearTimer(WaveTimerHandle);
    CurrentWaveIndex++;

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (USpartaGameInstance* SpartaGameInstance =
            Cast<USpartaGameInstance>(GameInstance))
        {
            // AddScore(Score);
            SpartaGameInstance->CurrentWaveIndex = CurrentWaveIndex;
        }
    }
    if (CurrentWaveIndex >= MaxWave)
    {
        EndLevel();
    }
    else
    {
        bIsWaveWating = true;
        GetWorldTimerManager().SetTimer(
            WaveTimerHandle,
            this,
            &ASpartaGameState::StartWave,
            WaveWaitDuration,
            false
        );
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
        EndWave();
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
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
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
            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
                {
                    if (bIsWaveWating)
                    {
                        LevelText->SetText(FText::FromString(FString::Printf(TEXT("Wating..."))));
                    }
                    else
                    {
                        LevelText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), CurrentWaveIndex + 1)));
                    }
                }
            }

            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                if (UTextBlock* CoinCntText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CoinCntText"))))
                {
                    if (bIsWaveWating)
                    {
                        CoinCntText->SetText(FText::FromString(FString::Printf(TEXT(": 0 / 0"))));
                    }
                    else
                    {
                        CoinCntText->SetText(FText::FromString(FString::Printf(TEXT(": %d / %d"), CollectedCoinCnt, SpawnedCoinCnt)));
                    }
                }
            }
        }
    }

}
