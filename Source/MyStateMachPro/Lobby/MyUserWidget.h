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
		bool CreateRoom(int timeValue, int roundValue, const FString& name);
	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool JoinRoom(int timeValue, int roundValue, const FString& name);
	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool LeaveRoom();
	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool CreateClient();
	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool ElementUpdateLobby(int slot1Pos, int slot2Pos, bool ready);



	

		UFUNCTION(BlueprintImplementableEvent, Category = "ServerMessages")
	void CreateRoomMessage(bool status);
		UFUNCTION(BlueprintImplementableEvent, Category = "ServerMessages")
	void JoinRoomMessage(bool status, const FString& name);
		UFUNCTION(BlueprintImplementableEvent, Category = "ServerMessages")
	void RivalJoinMessage(const FString& name);
		UFUNCTION(BlueprintImplementableEvent, Category = "ServerMessages")
	void LeaveRoomMessage(bool status);
		UFUNCTION(BlueprintImplementableEvent, Category = "ServerMessages")
	void RivalLeaveMessage();
		UFUNCTION(BlueprintImplementableEvent, Category = "ServerMessages")
	void UpdateLobbyValues(bool owner, int slot1Pos, int slot2Pos, bool ready);
	
};
