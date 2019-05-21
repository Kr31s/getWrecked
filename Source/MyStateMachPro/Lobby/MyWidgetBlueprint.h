// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBlueprint.h"
#include "MyWidgetBlueprint.generated.h"

/**
 *
 */
UCLASS()
class MYSTATEMACHPRO_API UMyWidgetBlueprint : public UWidgetBlueprint
{
	GENERATED_BODY()
public :

		UFUNCTION(BlueprintImplementableEvent)
		bool CreateRoom(const FString& p_Name);

	UFUNCTION(BlueprintImplementableEvent)
		bool JoinRoom(const FString& p_Name);

	UFUNCTION(BlueprintImplementableEvent)
		bool LeaveRoom(const FString& p_Name);

};
