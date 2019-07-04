// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUIClass.generated.h"

UCLASS()
class MYSTATEMACHPRO_API UInGameUIClass : public UUserWidget
{
	GENERATED_BODY()
	
public:
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InGameUI")
		void PauseGameUI();
};
