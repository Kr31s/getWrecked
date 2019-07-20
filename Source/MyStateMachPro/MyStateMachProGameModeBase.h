// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FGDefaultPawn.h"
#include "Lobby/NetworkSystem.h"
#include "MyStateMachProGameModeBase.generated.h"


/**
 *
 */

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

	static unsigned int sFrameCounter;

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

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
		bool scoreFlag;

	UPROPERTY()
		float startTimer;

	UPROPERTY(BlueprintReadWrite)
		int roundNumber;
	
	UPROPERTY(EditAnywhere)
		float prepTime = 3;

	UFUNCTION()
		void SetupMatch();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float transitiontime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float transitionMaxDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float transitionSpeed;

	UFUNCTION()
		void CheckOnWhichSidePlayerIs();

	UFUNCTION()
		void DetermineMatchWinner();

	UPROPERTY(BlueprintReadWrite, Category = "Score")
		int player1Score = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Score")
		int player2Score = 0;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChangedSignature, float, newTime);
	UPROPERTY(BlueprintAssignable)
		FOnTimeChangedSignature OnTimeChanged;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchNumberChangedSignature, int, newNumber);
	UPROPERTY(BlueprintAssignable)
		FOnMatchNumberChangedSignature OnMatchNumberChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnP1ScoreChangedSignature, int, newNumber);
	UPROPERTY(BlueprintAssignable)
		FOnP1ScoreChangedSignature OnP1ScoreChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnP2ScoreChangedSignature, int, newNumber);
	UPROPERTY(BlueprintAssignable)
		FOnP2ScoreChangedSignature OnP2ScoreChanged;

	UFUNCTION()
	void SetRoundTimer(float deltaSeconds);

	UFUNCTION()
		void RoundTimeOver();
};
