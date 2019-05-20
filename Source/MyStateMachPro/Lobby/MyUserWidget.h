// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

UCLASS()
class MYSTATEMACHPRO_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool CreateRoom(const FString& p_Name);

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool JoinRoom(const FString& p_Name);

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool LeaveRoom(const FString& p_Name);
};
