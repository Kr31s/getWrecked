#include "NetworkSystem.h"
#include "MyUserWidget.h"

NetworkSystem* NetworkSystem::NetSys = NULL;

bool NetworkSystem::StartingMessageReceiveThread()
{
	MessageReceiveThread = FMessageReceiveThread::InitThread(&socketUDP, m_receiveArray);

	return false;
}

NetworkSystem::NetworkSystem()
{

}


NetworkSystem::~NetworkSystem()
{
}

void NetworkSystem::TaskMessageReceiveThread(char* p_receiveArray)
{
	identifier = p_receiveArray[0] >> 1;
	status = p_receiveArray[0] << 7;
	status = status >> 7;

	UE_LOG(LogTemp, Warning, TEXT("%d"), (int)identifier);
	unsigned int frameValue = 0;
	unsigned short inputValue = 0;

	switch (identifier)
	{
	case 0:
		if (status)
		{
			for (int i = 0; (i < 20) && (p_receiveArray[i + 2] != -52); i++)
			{
				opponentName[i] = p_receiveArray[i + 2];
			}

			myRoomID = p_receiveArray[1];

			roomOwner = false;
		}
		else
		{
			//failed room join
		}


		UMyUserWidget::myUserWidget->JoinRoomMessage((bool)status, FString(UTF8_TO_TCHAR(opponentName)));
		break;

	case 1:
		sendArray[0] = identifier;
		for (int i = 0; (i < 20) && (p_receiveArray[i + 1] != -52); i++)
		{
			opponentName[i] = p_receiveArray[i + 1];
		}
		UMyUserWidget::myUserWidget->RivalJoinMessage(FString(UTF8_TO_TCHAR(opponentName)));
		UE_LOG(LogTemp, Warning, TEXT("RivalJoinMessage"));
		SendReceiveMessageClient();

		break;

	case 2:
		if (status)
		{
			myRoomID = p_receiveArray[1];
			roomOwner = true;
		}
		else
		{
			//failed room creation
		}
		if (UMyUserWidget::myUserWidget != NULL) {

			UMyUserWidget::myUserWidget->CreateRoomMessage((bool)status);
		}
		break;

	case 3:
		if (status)
		{
			//room left
			myRoomID = NULL;
		}
		else
		{
			//you in no room

		}
		UMyUserWidget::myUserWidget->LeaveRoomMessage((bool)status);
		break;

	case 4:
		//rival left room
		sendArray[0] = identifier;
		UMyUserWidget::myUserWidget->RivalLeaveMessage();

		SendReceiveMessageClient();
		break;


	case 5:
		heartBeatArray[0] = p_receiveArray[1];
		heartBeatArray[0] = heartBeatArray[0] << 1;
		heartBeatArray[0] |= static_cast<char>(1);
		UE_LOG(LogTemp, Warning, TEXT("heart beat"));

		socketUDP.Send(serverAddress, (char*)heartBeatArray, 1).m_errorCode;
		break;

	case 7:
		sendArray[0] = identifier;
		sendArray[1] = p_receiveArray[1] << 6;
		sendArray[1] = sendArray[1] >> 6;
		sendArray[2] = p_receiveArray[1] << 4;
		sendArray[2] = sendArray[2] >> 6;
		sendArray[3] = p_receiveArray[1] << 3;
		sendArray[3] = sendArray[3] >> 7;

		UMyUserWidget::myUserWidget->UpdateLobbyValues(roomOwner, (int)sendArray[1], (int)sendArray[2], (bool)sendArray[3]);
		SendReceiveMessageClient();
		break;

	case 11:


		for (int i = 0; i < 14; ++i)
		{
			if (i % 2 == 0)
			{
				frameValue = ((int)p_receiveArray[1 + i * 2] << 4);
				frameValue |= (int)(p_receiveArray[1 + i * 2 + 1] >> 4);

				inputValue = (int)(p_receiveArray[1 + i * 2 + 1] << 4);
				inputValue |= (int)(p_receiveArray[1 + i * 2 + 2]);
			}
			else
			{
				frameValue = ((int)p_receiveArray[1 + i * 2 + 1] << 4);
				frameValue |= (int)(p_receiveArray[1 + i * 2 + 2] >> 4);

				inputValue = ((int)p_receiveArray[1 + i * 2 + 2] >> 4);
				inputValue = inputValue << 4;
				inputValue |= (int)(p_receiveArray[1 + i * 2 + 3]);
			}
			
			if(receivedInputs.find(frameValue) != receivedInputs.end())
			{
				receivedInputs.insert({ frameValue, inputValue });
			}
		}

		SendReceiveMessageClient();
		break;

	default:
		//unknown command
		break;
	}

}

bool NetworkSystem::InitNetSystem()
{
	this->serverAddress = NetAddress(127, 0, 0, 1, 4405);

	BWNet::InitializeSocketLayer();

	if (socketUDP.OpenSocket(0).m_errorCode == 0)
	{
		if (socketUDP.EnableNonBlocking().m_errorCode == 0)
		{
			this->StartingMessageReceiveThread();
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void NetworkSystem::SendReceiveMessageClient()
{
	sendArray[0] = sendArray[0] << 1;
	sendArray[0] |= static_cast<char>(1);
	socketUDP.Send(serverAddress, (char*)sendArray, 1).m_errorCode;
	ClearReceiveArray();
}

void NetworkSystem::ClearReceiveArray()
{
	int a = sizeof(sendArray) / sizeof(*sendArray);
	for (int i = 0; i < a; ++i)
	{
		m_receiveArray[i] = 0;
		sendArray[i] = 0;
	}
}
