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

	return false;

}

void  UMyUserWidget::ReceiveThread()
{
	while (socketOnline)
	{
		if (socketUDP.Receive(receiveArray, 50).GetPortRef() != NULL)
		{
			ReceiveMessageClient();
		}
		UE_LOG(LogTemp, Warning, TEXT("Thread working"));
	}

	UE_LOG(LogTemp, Warning, TEXT("End of thread"));

}

void  UMyUserWidget::SendRequestClient(int messageType)
{
	this->ClearReceiveArray();

	switch (messageType)
	{
	case 0:
		//charInput Name
		for (int i = 2; i < 22; ++i)
		{
			sendArray[i] = charInput[i - 2];
		}
		//input round settings
		sendArray[0] = 0;
		sendArray[1] = input << 5;


		//input time settings
		sendArray[1] |= input << 2;

		socketUDP.Send(serverAddress, (char*)sendArray, 22).m_errorCode;
		break;

	case 2:
		//charInput Name

		for (int i = 2; i < 22; ++i)
		{
			sendArray[i] = charInput[i - 2];
		}
		//input round settings
		sendArray[0] = 2 << 1;
		sendArray[0] = 2 << 1;
		sendArray[1] = input << 5;

		//input time settings
		sendArray[1] |= input << 2;

		socketUDP.Send(serverAddress, (char*)sendArray, 22).m_errorCode;
		break;

	case 3:
		if (myRoomID >= 0)
		{
			sendArray[0] = 3 << 1;
			sendArray[1] = myRoomID;

			socketUDP.Send(serverAddress, (char*)sendArray, 2).m_errorCode;
			break;
		}
		//her in no room
		break;
	default:
		//unkown kommand
		break;
	}
}

void  UMyUserWidget::ReceiveMessageClient()
{
	identifier = receiveArray[0] >> 1;
	status = receiveArray[0] << 7;
	status = status >> 7;


	switch (identifier)
	{
	case 0:
		sendArray[0] = receiveArray[22];

		if (status)
		{
			for (int i = 0; (i < 20) && (receiveArray[i + 2] != -52); i++)
			{
				opponentName[i] = receiveArray[i + 2];
			}

			myRoomID = receiveArray[1];
		}
		else
		{
		}
		break;

	case 1:
		sendArray[0] = receiveArray[21];
		for (int i = 0; (i < 20) && (receiveArray[i + 1] != -52); i++)
		{
			opponentName[i] = receiveArray[i + 1];
		}
		SendReceiveMessageClient();

		break;

	case 2:
		sendArray[0] = receiveArray[2];
		if (status)
		{
			myRoomID = receiveArray[1];
		}
		else
		{
		}
		break;

	case 3:
		if (status)
		{
				myRoomID = NULL;
		}
		else
		{
		}
		break;

	case 4:
		sendArray[0] = receiveArray[1];
		SendReceiveMessageClient();
		break;


	case 5:
		heartBeatArray[0] = receiveArray[1];
		heartBeatArray[0] = heartBeatArray[0] << 1;
		heartBeatArray[0] |= static_cast<char>(1);
		socketUDP.Send(serverAddress, (char*)heartBeatArray, 1).m_errorCode;

		break;

	default:
		myRoomID = NULL;
		break;
	}
}

void  UMyUserWidget::SendReceiveMessageClient()
{
	sendArray[0] = sendArray[0] << 1;
	sendArray[0] |= static_cast<char>(1);
	socketUDP.Send(serverAddress, (char*)sendArray, 1).m_errorCode;
}

void  UMyUserWidget::ClearReceiveArray()
{
	int a = 21;
	for (int i = 0; i < a; ++i)
	{
		charInput[i] = 0;
	}

	a = sizeof(sendArray) / sizeof(*sendArray);
	for (int i = 0; i < a; ++i)
	{
		receiveArray[i] = 0;
		sendArray[i] = 0;
	}
}
