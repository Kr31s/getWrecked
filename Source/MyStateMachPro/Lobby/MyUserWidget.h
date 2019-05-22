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

	NetSocketUDP socketUDP;
	NetAddress serverAddress;
	NetAddress receiveAddress;
	bool socketOnline = true;

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

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
		bool CreateClient();


private:


	void ReceiveThread();

	void SendRequestClient(int messageType);

	void ReceiveMessageClient();

	void SendReceiveMessageClient();

	void ClearReceiveArray();
};
