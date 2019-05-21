// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "AllowWindowsPlatformTypes.h"
//#include "HideWindowsPlatformTypes.h"
//
//#include "../ThirdParty/BWLib/include/Inc_BWMath.h"
//#include "../ThirdParty/BWLib/include/Inc_BWNetworking.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

UCLASS()
class MYSTATEMACHPRO_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool CreateRoom(int TimeValue, int RoundValue, const FString& p_Name);

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool JoinRoom(int TimeValue, int RoundValue, const FString& p_Name);

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool LeaveRoom();
};
