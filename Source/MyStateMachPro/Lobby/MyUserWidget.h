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
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool CreateRoom(const FString& p_Name);

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool JoinRoom(const FString& p_Name);

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool LeaveRoom();

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool CreateClient();


private:


	void SendRequestClient(unsigned int messageType, unsigned int roundSettingsAdjusted, unsigned int timeSettingsAdjusted, char* nickname);

	void ReceiveMessageClient();

	void SendReceiveMessageClient();


	
};
