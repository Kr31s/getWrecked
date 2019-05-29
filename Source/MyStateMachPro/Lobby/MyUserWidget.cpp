// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "NetworkSystem.h"
#include "OutputDeviceNull.h"

UMyUserWidget* UMyUserWidget::myUserWidget = NULL;

void UMyUserWidget::BeginDestroy()
{
	Super::BeginDestroy();
	FMessageReceiveThread::threadRuning = false;
	NetworkSystem::NetSys->MessageReceiveThread->Shutdown();
}

bool UMyUserWidget::CreateRoom(int p_timeValue, int p_roundValue, const FString& p_Name)
{
	UMyUserWidget::myUserWidget = this;

	UE_LOG(LogTemp, Warning, TEXT("CreateRoom"));
	char* result = TCHAR_TO_ANSI(*p_Name);
	for (int i = 2; i < 22; ++i)
	{
		NetworkSystem::NetSys->sendArray[i] = result[i - 2];
	}
	NetworkSystem::NetSys->sendArray[0] = 2 << 1;

	//input round settings
	NetworkSystem::NetSys->sendArray[1] = p_roundValue << 5;
	//input time settings
	NetworkSystem::NetSys->sendArray[1] |= p_timeValue << 2;

	NetworkSystem::NetSys->socketUDP.Send(NetworkSystem::NetSys->serverAddress, (char*)NetworkSystem::NetSys->sendArray, 22).m_errorCode;
	return true;
}



bool UMyUserWidget::JoinRoom(int p_timeValue, int p_roundValue, const FString& p_Name)
{
	UE_LOG(LogTemp, Warning, TEXT("JoinRoom"));
	//charInput Name
	char* result = TCHAR_TO_ANSI(*p_Name);
	for (int i = 2; i < 22; ++i)
	{
		NetworkSystem::NetSys->sendArray[i] = result[i - 2];
	}
	NetworkSystem::NetSys->sendArray[0] = 0;

	//input round settings
	NetworkSystem::NetSys->sendArray[1] = p_roundValue << 5;
	//input time settings
	NetworkSystem::NetSys->sendArray[1] |= p_timeValue << 2;

	NetworkSystem::NetSys->socketUDP.Send(NetworkSystem::NetSys->serverAddress, (char*)NetworkSystem::NetSys->sendArray, 22).m_errorCode;
	return true;
}

bool UMyUserWidget::LeaveRoom()
{
	if (NetworkSystem::NetSys->myRoomID >= 0)
	{
		NetworkSystem::NetSys->sendArray[0] = 3 << 1;
		NetworkSystem::NetSys->sendArray[1] = NetworkSystem::NetSys->myRoomID;

		NetworkSystem::NetSys->socketUDP.Send(NetworkSystem::NetSys->serverAddress, (char*)NetworkSystem::NetSys->sendArray, 2).m_errorCode;
		return true;
	}
	return false;
}

bool  UMyUserWidget::CreateClient()
{
	UE_LOG(LogTemp, Warning, TEXT("CreateClient"));

	myUserWidget = this;

	NetworkSystem::NetSys = new NetworkSystem();

	return NetworkSystem::NetSys->InitNetSystem();
}

//void  UMyUserWidget::CreateRoomMessage_Implementation(bool status)
//{
//	int a = 5;
//	int b = 5;
//	UE_LOG(LogTemp, Warning, TEXT("Coolio"));
//}

void  UMyUserWidget::JoinRoomMessage_Implementation(bool status)
{

}
void  UMyUserWidget::RivalJoinMessage_Implementation()
{

}
void  UMyUserWidget::LeaveRoomMessage_Implementation(bool status)
{

}
void  UMyUserWidget::RivalLeaveMessage_Implementation()
{

}
