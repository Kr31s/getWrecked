// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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
	virtual void Tick(float Deltaseconds) override;

	void SpawnSecondPlayer();



	// This will be spawned when the game starts.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ACameraActor* MainGameCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> wMainMenu;
	
	UPROPERTY()
		UUserWidget* MyMainMenu;
};
