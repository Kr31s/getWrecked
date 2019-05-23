// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "NetworkSystem.h"

void UMyUserWidget::BeginDestroy()
{
	Super::BeginDestroy();
	FMessageReceiveThread::threadRuning = false;
	NetworkSystem::NetSys->MessageReceiveThread->Shutdown();
}

bool UMyUserWidget::CreateRoom(int p_timeValue, int p_roundValue, const FString& p_Name)
{

	UE_LOG(LogTemp, Warning, TEXT("CreateRoom"));
	for (int i = 2; i < 22; ++i)
	{
		NetworkSystem::NetSys->sendArray[i] = p_Name[i - 2];
	}
	//input round settings
	NetworkSystem::NetSys->sendArray[0] = 2 << 1;
	NetworkSystem::NetSys->sendArray[1] = p_roundValue << 5;

	//input time settings
	NetworkSystem::NetSys->sendArray[1] |= p_timeValue << 2;

	UE_LOG(LogTemp, Warning, TEXT("destructor"));
	NetworkSystem::NetSys->socketUDP.Send(NetworkSystem::NetSys->serverAddress, (char*)NetworkSystem::NetSys->sendArray, 22).m_errorCode;
	UE_LOG(LogTemp, Warning, TEXT("destructor2"));
	return true;
}

bool UMyUserWidget::JoinRoom(int p_timeValue, int p_roundValue, const FString& p_Name)
{
	//charInput Name
	for (int i = 2; i < 22; ++i)
	{
		NetworkSystem::NetSys->sendArray[i] = p_Name[i - 2];
	}
	//input round settings
	NetworkSystem::NetSys->sendArray[0] = 0;
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

	NetworkSystem::NetSys = new NetworkSystem();
	UE_LOG(LogTemp, Warning, TEXT("NetworkSystem"));

	return NetworkSystem::NetSys->InitNetSystem();
}

void  UMyUserWidget::SendRequestClient(unsigned int messageType, unsigned int roundSettingsAdjusted, unsigned int timeSettingsAdjusted, char* nickname)
{
	NetworkSystem::NetSys->ClearReceiveArray();

	switch (messageType)
	{
	case 0:
		break;

	case 2:
		//charInput Name
break;

	case 3:
		
		//her in no room
		break;
	default:
		//unkown kommand
		break;
	}
}

void  UMyUserWidget::ReceiveMessageClient()
{
	NetworkSystem::NetSys->identifier = NetworkSystem::NetSys->m_receiveArray[0] >> 1;
	NetworkSystem::NetSys->status = NetworkSystem::NetSys->m_receiveArray[0] << 7;
	NetworkSystem::NetSys->status = NetworkSystem::NetSys->status >> 7;


	switch (NetworkSystem::NetSys->identifier)
	{
	case 0:
		NetworkSystem::NetSys->sendArray[0] = NetworkSystem::NetSys->m_receiveArray[22];

		if (NetworkSystem::NetSys->status)
		{
			for (int i = 0; (i < 20) && (NetworkSystem::NetSys->m_receiveArray[i + 2] != -52); i++)
			{
				NetworkSystem::NetSys->opponentName[i] = NetworkSystem::NetSys->m_receiveArray[i + 2];
			}

			NetworkSystem::NetSys->myRoomID = NetworkSystem::NetSys->m_receiveArray[1];
		}
		else
		{
		}
		break;

	case 1:
		NetworkSystem::NetSys->sendArray[0] = NetworkSystem::NetSys->m_receiveArray[21];
		for (int i = 0; (i < 20) && (NetworkSystem::NetSys->m_receiveArray[i + 1] != -52); i++)
		{
			NetworkSystem::NetSys->opponentName[i] = NetworkSystem::NetSys->m_receiveArray[i + 1];
		}
		SendReceiveMessageClient();

		break;

	case 2:
		NetworkSystem::NetSys->sendArray[0] = NetworkSystem::NetSys->m_receiveArray[2];
		if (NetworkSystem::NetSys->status)
		{
			NetworkSystem::NetSys->myRoomID = NetworkSystem::NetSys->m_receiveArray[1];
		}
		else
		{
		}
		break;

	case 3:
		if (NetworkSystem::NetSys->status)
		{
			NetworkSystem::NetSys->myRoomID = NULL;
		}
		else
		{
		}
		break;

	case 4:
		NetworkSystem::NetSys->sendArray[0] = NetworkSystem::NetSys->m_receiveArray[1];
		SendReceiveMessageClient();
		break;


	case 5:
		NetworkSystem::NetSys->heartBeatArray[0] = NetworkSystem::NetSys->m_receiveArray[1];
		NetworkSystem::NetSys->heartBeatArray[0] = NetworkSystem::NetSys->heartBeatArray[0] << 1;
		NetworkSystem::NetSys->heartBeatArray[0] |= static_cast<char>(1);
		NetworkSystem::NetSys->socketUDP.Send(NetworkSystem::NetSys->serverAddress, (char* )NetworkSystem::NetSys->heartBeatArray, 1).m_errorCode;

		break;

	default:
		NetworkSystem::NetSys->myRoomID = NULL;
		break;
	}
}

void  UMyUserWidget::SendReceiveMessageClient()
{
	NetworkSystem::NetSys->sendArray[0] = NetworkSystem::NetSys->sendArray[0] << 1;
	NetworkSystem::NetSys->sendArray[0] |= static_cast<char>(1);
	NetworkSystem::NetSys->socketUDP.Send(NetworkSystem::NetSys->serverAddress, (char*)NetworkSystem::NetSys->sendArray, 1).m_errorCode;
}

