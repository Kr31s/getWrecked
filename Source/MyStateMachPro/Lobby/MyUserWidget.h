// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AllowWindowsPlatformTypes.h"
#include <thread>
#include "HideWindowsPlatformTypes.h"

#include "../ThirdParty/BWLib/include/Inc_BWMath.h"
#include "../ThirdParty/BWLib/include/Inc_BWNetworking.h"
#include "../ThirdParty/BWLib/include/Inc_SmartMacros.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"




UCLASS()
class MYSTATEMACHPRO_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

	std::thread receiveT;

	NetSocketUDP socketUDP;
	NetAddress serverAddress;
	NetAddress receiveAddress;

	unsigned char opponentName[20];

	unsigned char sendArray[51];
	unsigned char heartBeatArray[51];
	char receiveArray[51];

	int myRoomID = -1;
	unsigned char identifier = NULL;
	unsigned char status = NULL;
	int input;
	char* charInput = new char[20];

public:

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool CreateRoom(int TimeValue, int RoundValue, const FString& p_Name);

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool JoinRoom(int TimeValue, int RoundValue, const FString& p_Name);

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool LeaveRoom();


private:

	bool CreateClient();

	void ReceiveThread();

	void SendRequestClient(int messageType, NetSocketUDP socketUDP);

	void ReceiveMessageClient();

	void SendReceiveMessageClient();

	void ClearReceiveArray();
};
