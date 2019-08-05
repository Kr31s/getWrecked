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
	static bool hasGameStarted;
	static int m_roundVal;
	static int m_timeVal;
	static FString m_opponentName;//FString(UTF8_TO_TCHAR(opponentName))
	static FString m_playerName;

	static unsigned int m_framesToSync;


	static unsigned int sFrameCounter;

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

	AMyStateMachProGameModeBase();
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MatchCount")
		EMatcheTypes MatchCount;

	UPROPERTY()
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AFGDefaultPawn* player1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString player1Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString player2Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AFGDefaultPawn* player2;

	UPROPERTY(EditAnywhere)
		float roundTimer;

	UPROPERTY()
		bool enableInputOnRoundStart;

	UPROPERTY()
		bool scoreFlag;

	UPROPERTY()
		bool playFadeoutFlag = true;

	UPROPERTY()
		bool isMatchOver;

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
		float roundTransitionMaxDuration;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float slowmotionMaxDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float transitionSpeed;

	UFUNCTION()
		void CheckOnWhichSidePlayerIs();

	UFUNCTION()
		void DetermineMatchWinner();

	UFUNCTION()
		void CheckIfMatchIsOver(int playerScore);

	UPROPERTY(BlueprintReadWrite, Category = "Score")
		int player1Score = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Score")
		int player2Score = 0;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChangedSignature, float, newTime);
	UPROPERTY(BlueprintAssignable)
		FOnTimeChangedSignature OnTimeChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnName1ChangedSignature, FString, name);
	UPROPERTY(BlueprintAssignable)
		FOnName1ChangedSignature OnName1Changed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnName2ChangedSignature, FString, name);
	UPROPERTY(BlueprintAssignable)
		FOnName2ChangedSignature OnName2Changed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchNumberChangedSignature, EMatcheTypes, newNumber);
	UPROPERTY(BlueprintAssignable)
		FOnMatchNumberChangedSignature OnMatchNumberChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnP1ScoreChangedSignature, int, newNumber);
	UPROPERTY(BlueprintAssignable)
		FOnP1ScoreChangedSignature OnP1ScoreChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnP2ScoreChangedSignature, int, newNumber);
	UPROPERTY(BlueprintAssignable)
		FOnP2ScoreChangedSignature OnP2ScoreChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOnNextRoundNumberSignature, int, newNumber);
	UPROPERTY(BlueprintAssignable)
		FOnOnNextRoundNumberSignature OnNextRoundNumber;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchIsOverSignature, bool, isOnline, bool, isPlayer1Winner);
	UPROPERTY(BlueprintAssignable)
		FOnMatchIsOverSignature OnMatchIsOverCheckIfOnline;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStartedSignature, bool, hasGameStarted);
	UPROPERTY(BlueprintAssignable)
		FOnGameStartedSignature OnGameStarted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFadeOutSignature, bool, playFadeOut);
	UPROPERTY(BlueprintAssignable)
		FOnFadeOutSignature OnFadeOut;

	UFUNCTION()
	void SetRoundTimer(float deltaSeconds);

	UFUNCTION(BlueprintImplementableEvent)
		void ResetVictoryMontage();

	UFUNCTION(BlueprintImplementableEvent)
		void ResetLoosingMontage();

	UFUNCTION()
		void RoundTimeOver(float DeltaSeconds);
};
