// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FGDefaultPawn.h"
#include "MyStateMachProGameModeBase.generated.h"

/**
 *
 */

struct matchStats
{
	int player1Score = 0;
	int player2Score = 0;
};

UENUM(BlueprintType)
enum class EMatcheTypes : uint8 {
	BestofOne UMETA(DisplayName = "BestofOne"),
	BestofThree UMETA(DisplayName = "BestofThree"),
	BestofFive UMETA(DisplayName = "BestofFive")
};

UCLASS()
class MYSTATEMACHPRO_API AMyStateMachProGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


public:

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	AMyStateMachProGameModeBase();
	void SpawnSecondPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MatchCount")
		EMatcheTypes MatchCount;

	UPROPERTY(EditAnywhere)
		float roundTime;
	// This will be spawned when the game starts.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ACameraActor* MainGameCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> wMainMenu;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundTimer")
		//uint32 matchTime;

	UPROPERTY()
		UUserWidget* MyMainMenu;

	UPROPERTY()
		AFGDefaultPawn* player1;

	UPROPERTY()
		AFGDefaultPawn* player2;

	UPROPERTY(EditAnywhere)
		float roundTimer;

	UPROPERTY()
		float startTimer;	
	
	UPROPERTY(EditAnywhere)
		float prepTime = 3;

	UPROPERTY()
		FVector2D matchStanding;

	UFUNCTION()
		void SetupMatch();


	UFUNCTION()
		void CheckOnWhichSidePlayerIs();

	UFUNCTION()
		void DetermineMatchWinner();

	matchStats Standings;
};
