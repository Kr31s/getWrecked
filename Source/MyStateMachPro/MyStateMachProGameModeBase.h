// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FGDefaultPawn.h"
#include "MyStateMachProGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class MYSTATEMACHPRO_API AMyStateMachProGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


public:

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	AMyStateMachProGameModeBase();
	void SpawnSecondPlayer();



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
};
