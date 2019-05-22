// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"


bool UMyUserWidget::CreateRoom(int TimeValue, int RoundValue, const FString& p_Name)
{
	return false;
}

bool UMyUserWidget::JoinRoom(int TimeValue, int RoundValue, const FString& p_Name)
{
	return false;
}

bool UMyUserWidget::LeaveRoom()
{
	return false;
}


bool  UMyUserWidget::CreateClient()
{
	this->serverAddress = NetAddress(127, 0, 0, 1, 4405);

	BWNet::InitializeSocketLayer();
	this->receiveT = std::thread(&UMyUserWidget::ReceiveThread, this);

	if (socketUDP.OpenSocket(0).m_errorCode == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Success Socket Open"));
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Socket Open"));
	}

	return false;

}

void  UMyUserWidget::ReceiveThread()
{

}

void  UMyUserWidget::SendRequestClient(int messageType, NetSocketUDP socketUDP)
{

}

void  UMyUserWidget::ReceiveMessageClient()
{

}

void  UMyUserWidget::SendReceiveMessageClient()
{

}

void  UMyUserWidget::ClearReceiveArray()
{

}