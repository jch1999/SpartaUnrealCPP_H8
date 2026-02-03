// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAUNREALCPP_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()
public:
	ASpartaGameState();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Mine")
	void IncreaseExplodeCnt();

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	void StartLevel();
	void EndLevel();

	void StartWave();
	void OnWaveTimeUP();
	void EndWave();

	void OnCoinCollected();

	void UpdateHUD();

	void SetDizzy(bool IsDizzy);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCnt;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCnt;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mine")
	int32 ExplodeCnt;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mine")
	int32 MaxExplodeCnt;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")

	int32 MaxLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWave;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<float> WaveDurationArray;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float WaveWaitDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	bool bIsWaveWating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<int32> ItemMaxSpawnCntPerWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BadStatus")
	bool bIsDizzy;

	FTimerHandle LevelTimerHandle;
	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
};
