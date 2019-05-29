// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "AllowWindowsPlatformTypes.h"
#include "HideWindowsPlatformTypes.h"

#include "../ThirdParty/BWLib/include/Inc_BWNetworking.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"



UCLASS()
class MYSTATEMACHPRO_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	static UMyUserWidget* myUserWidget;

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool CreateRoom(int TimeValue, int RoundValue, const FString& p_Name);

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool JoinRoom(int TimeValue, int RoundValue, const FString& p_Name);

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool LeaveRoom();

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool CreateClient();



	

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ZZZ")
	void CreateRoomMessage(bool status);
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ZZZ")
	void JoinRoomMessage(bool status);
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ZZZ")
	void RivalJoinMessage();
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ZZZ")
	void LeaveRoomMessage(bool status);
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ZZZ")
	void RivalLeaveMessage();
	
};
