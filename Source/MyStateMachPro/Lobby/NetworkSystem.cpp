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
		this->RoomRequestAnswer(status, p_receiveArray);
		break;
	case 1:
		this->RoomJoin(p_receiveArray);
		break;
	case 2:
		this->CreateRoomAnswer(status, p_receiveArray);
		break;
	case 4:
		this->OpponentLeftRoom(p_receiveArray);
		break;
	case 5:
		this->Hearthbeat(p_receiveArray);
		break;
	case 7:
		this->ElementUpdate(p_receiveArray);
		break;
	case 9:
		this->PauseGameUpdate(status, p_receiveArray);
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

void NetworkSystem::RoomRequest(int& p_timeValue, int& p_roundValue, const FString& p_name)
{
	UE_LOG(LogTemp, Warning, TEXT("RoomRequest"));
	//charInput Name
	char* result = TCHAR_TO_ANSI(*p_name);
	for (int i = 2; i < 22; ++i)
	{
		sendArray[i] = result[i - 2];
	}
	sendArray[0] = 0;

	//input round settings
	sendArray[1] = p_roundValue << 5;
	//input time settings
	sendArray[1] |= p_timeValue << 2;

	socketUDP.Send(serverAddress, (char*)sendArray, 22).m_errorCode;

}
void NetworkSystem::CreateRoom(int& p_timeValue, int& p_roundValue, const FString& p_name)
{
	UE_LOG(LogTemp, Warning, TEXT("CreateRoom"));
	char* result = TCHAR_TO_ANSI(*p_name);

	for (int i = 2; i < 22; ++i)
	{
		sendArray[i] = result[i - 2];
	}
	sendArray[0] = 2 << 1;

	//input round settings
	sendArray[1] = p_roundValue << 5;
	//input time settings
	sendArray[1] |= p_timeValue << 2;

	socketUDP.Send(serverAddress, (char*)sendArray, 22).m_errorCode;

}
void NetworkSystem::LeaveRoom()
{
	if (myRoomID >= 0)
	{
		sendArray[0] = 3 << 1;
		sendArray[1] = myRoomID;

		socketUDP.Send(serverAddress, (char*)sendArray, 2);
	}
}
void NetworkSystem::ElementChanged(int& slot1Pos, int& slot2Pos, bool& ready)
{
	sendArray[0] = 6 << 1;
	sendArray[1] = myRoomID;
	sendArray[2] = slot1Pos;
	sendArray[2] |= slot2Pos << 2;
	sendArray[2] |= ready << 4;

	socketUDP.Send(serverAddress, (char*)sendArray, 3);
}
void NetworkSystem::PauseGame(bool& stop)
{
}
void NetworkSystem::GameMessage(std::bitset<12>& inputStream)
{
}

void NetworkSystem::RoomRequestAnswer(unsigned char& status, char* p_receiveArray)
{
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

}
void NetworkSystem::RoomJoin(char* p_receiveArray)
{
	sendArray[0] = identifier;
	for (int i = 0; (i < 20) && (p_receiveArray[i + 1] != -52); i++)
	{
		opponentName[i] = p_receiveArray[i + 1];
	}
	UMyUserWidget::myUserWidget->RivalJoinMessage(FString(UTF8_TO_TCHAR(opponentName)));
	UE_LOG(LogTemp, Warning, TEXT("RivalJoinMessage"));
	SendReceiveMessageClient();
}
void NetworkSystem::CreateRoomAnswer(unsigned char& status, char* p_receiveArray)
{
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
}
void NetworkSystem::OpponentLeftRoom(char* p_receiveArray)
{
	sendArray[0] = identifier;
	UMyUserWidget::myUserWidget->RivalLeaveMessage();

	SendReceiveMessageClient();
}
void NetworkSystem::Hearthbeat(char* p_receiveArray)
{
	heartBeatArray[0] = p_receiveArray[1];
	heartBeatArray[0] = heartBeatArray[0] << 1;
	heartBeatArray[0] |= static_cast<char>(1);
	UE_LOG(LogTemp, Warning, TEXT("heart beat"));

	socketUDP.Send(serverAddress, (char*)heartBeatArray, 1).m_errorCode;
}
void NetworkSystem::ElementUpdate(char* p_receiveArray)
{
	sendArray[0] = identifier;
	sendArray[1] = p_receiveArray[1] << 6;
	sendArray[1] = sendArray[1] >> 6;
	sendArray[2] = p_receiveArray[1] << 4;
	sendArray[2] = sendArray[2] >> 6;
	sendArray[3] = p_receiveArray[1] << 3;
	sendArray[3] = sendArray[3] >> 7;

	UMyUserWidget::myUserWidget->UpdateLobbyValues(roomOwner, (int)sendArray[1], (int)sendArray[2], (bool)sendArray[3]);
	SendReceiveMessageClient();
}
void NetworkSystem::PauseGameUpdate(unsigned char& status, char* p_receiveArray)
{
}
void NetworkSystem::OppentGameMessage(char* p_receiveArray)
{
	for (int i = 0; i < 14; ++i)
	{
		if (i % 2 == 0)
		{
			frameValue = ((short)p_receiveArray[1 + i * 2] << 4);
			frameValue |= (short)(p_receiveArray[1 + i * 2 + 1] >> 4);

			inputValue = (short)(p_receiveArray[1 + i * 2 + 1] << 4);
			inputValue |= (short)(p_receiveArray[1 + i * 2 + 2]);
		}
		else
		{
			frameValue = ((short)p_receiveArray[1 + i * 2 + 1] << 4);
			frameValue |= (short)(p_receiveArray[1 + i * 2 + 2] >> 4);

			inputValue = ((short)p_receiveArray[1 + i * 2 + 2] >> 4);
			inputValue = inputValue << 4;
			inputValue |= (short)(p_receiveArray[1 + i * 2 + 3]);
		}

		if (receivedInputs.find(frameValue) != receivedInputs.end())
		{
			receivedInputs.insert({ frameValue, inputValue });
		}
	}

	SendReceiveMessageClient();
}
