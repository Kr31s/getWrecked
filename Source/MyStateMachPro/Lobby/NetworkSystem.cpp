#include "NetworkSystem.h"
#include "MyUserWidget.h"
#include "MyStateMachProGameModeBase.h"

#include <array>

NetworkSystem* NetworkSystem::NetSys = NULL;

bool NetworkSystem::StartingMessageReceiveThread() {
	MessageReceiveThread = FMessageReceiveThread::InitThread(&socketUDP, m_receiveArray);

	return false;
}
bool NetworkSystem::StartingResendMessageThread()
{
	ResendMessageThread = FResendMessageThread::InitThread(&socketUDP, m_receiveArray);
	return false;
}

NetworkSystem::~NetworkSystem()
{
}

NetworkSystem::NetworkSystem()
{

}

void NetworkSystem::setGameMode(AMyStateMachProGameModeBase* p_gameMode)
{
	m_gameMode = p_gameMode;
}

bool NetworkSystem::InitNetSystem()
{
	this->serverAddress = NetAddress(127, 0, 0, 1, 4023);

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
	identifier = p_receiveArray[0];
	status = p_receiveArray[1];

	switch (status)
	{
	case 1:
		SendReceiveMessageClient();
		break;
	case 2:
		//the massage we are waiting for can be deleted
		BCMessage::GetReplyMessage(p_receiveArray[45]);
		break;
	}


	switch (identifier)
	{
	case 0:
		this->RoomRequestAnswer(p_receiveArray);
		break;
	case 1:
		this->RoomJoin(p_receiveArray);
		break;
	case 2:
		this->CreateRoomAnswer(p_receiveArray);
		break;
	case 3:
		this->LeaveRoomAnswer(p_receiveArray);
		break;
	case 4:
		this->OpponentLeftRoom(p_receiveArray);
		break;
	case 7:
		this->ElementUpdate(p_receiveArray);
		break;
	case 9:
		this->PauseGameUpdate(p_receiveArray);
		break;
	case 11:
		this->OppentGameMessage(p_receiveArray);
		break;
	case 12:
		this->StartGame();;
		break;

	default:
		//unknown command
		break;
	}

}

void NetworkSystem::TaskResendMessageThread()
{
	for (unsigned int i = 0; i < BCMessage::sTotalMessageID; ++i)
	{
		sMutexMessageList.lock();
		if (m_messageIDList.find(i) == m_messageIDList.end())
		{
			continue;
		}
		if (m_messageIDList.at(i).m_finished)
		{
			m_messageIDList.erase(i);
			continue;
		}
		if (GetTimeInMilli() + 150 < m_messageIDList.at(i).m_timeStamp)
		{
			socketUDP.Send(serverAddress, m_messageIDList.at(i).m_messageArray, 46);
			m_messageIDList.at(i).m_finished = true;
		}
		sMutexMessageList.unlock();
	}
}

void NetworkSystem::ShutdownNetwork()
{
	FMessageReceiveThread::threadRuning = false;
	NetworkSystem::NetSys->MessageReceiveThread->Shutdown();
	delete(NetworkSystem::NetSys);
	NetworkSystem::NetSys = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("Thread Shutdown"));
}

void NetworkSystem::SendReceiveMessageClient()
{
	sendArray[0] = identifier;
	sendArray[1] = 2;
	sendArray[45] = m_receiveArray[45];
	socketUDP.Send(serverAddress, (char*)sendArray, 46).m_errorCode;
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

	for (int i = 0; i < 20; ++i)
	{
		if (i > p_name.Len() - 1)
		{
			sendArray[i + 4] = NULL;
		}
		else
		{
			sendArray[i + 4] = result[i];
		}
		UE_LOG(LogTemp, Warning, TEXT("%c"), result[i]);
	}

	sendArray[0] = 0;
	sendArray[1] = 0;

	//input round settings
	sendArray[2] = p_roundValue;
	//input time settings
	sendArray[3] = p_timeValue;
	sendArray[45] = BCMessage(sendArray).m_messageID;

	socketUDP.Send(serverAddress, (char*)sendArray, 46).m_errorCode;

}
void NetworkSystem::CreateRoom(int& p_timeValue, int& p_roundValue, const FString& p_name)
{
	UE_LOG(LogTemp, Warning, TEXT("CreateRoom"));
	char* result = TCHAR_TO_ANSI(*p_name);

	for (int i = 0; i < 20; ++i)
	{
		if (i > p_name.Len() - 1)
		{
			sendArray[i + 4] = NULL;
		}
		else
		{
			sendArray[i + 4] = result[i];
		}
		UE_LOG(LogTemp, Warning, TEXT("%c"), result[i]);
	}
	sendArray[0] = 2;
	sendArray[1] = 0;

	//input round settings
	sendArray[2] = p_roundValue;
	//input time settings
	sendArray[3] = p_timeValue;
	sendArray[45] = BCMessage(sendArray).m_messageID;


	socketUDP.Send(serverAddress, (char*)sendArray, 46).m_errorCode;

}
void NetworkSystem::LeaveRoom()
{
	if (myRoomID >= 0)
	{
		sendArray[0] = 3;
		sendArray[1] = 0;
		sendArray[2] = myRoomID;
		sendArray[45] = BCMessage(sendArray).m_messageID;

		socketUDP.Send(serverAddress, (char*)sendArray, 46);
	}
}
void NetworkSystem::ElementChanged(int& slot1Pos, int& slot2Pos, bool& ready)
{
	sendArray[0] = 6;
	sendArray[1] = 0;
	UE_LOG(LogTemp, Warning, TEXT("%d"), (int)myRoomID);

	sendArray[2] = myRoomID;
	sendArray[3] = slot1Pos;
	sendArray[4] = slot2Pos;
	sendArray[5] = ready;
	sendArray[45] = BCMessage(sendArray).m_messageID;

	socketUDP.Send(serverAddress, (char*)sendArray, 46);
}
void NetworkSystem::PauseGame(bool& stop)
{
	sendArray[0] = 8;
	sendArray[45] = BCMessage(sendArray).m_messageID;

	socketUDP.Send(serverAddress, (char*)sendArray, 46);
}
void NetworkSystem::GameMessage(std::bitset<12>& inputStream)
{
	unsigned short temp;
	sendArray[0] = 10;
	sendArray[1] = clientID;

	gameMessagesPlayer.insert(gameMessagesPlayer.begin(), GameMessageData(AMyStateMachProGameModeBase::sFrameCounter, (unsigned short)inputStream.to_ulong()));
	gameMessagesPlayer.resize(9);

	for (int i = 0; i < 9; ++i)
	{
		sendArray[2 + 4 * i] = (gameMessagesPlayer[i].m_time) >> 8;
		temp = (gameMessagesPlayer[i].m_time) << 8;
		sendArray[3 + 4 * i] = temp >> 8;
		sendArray[4 + 4 * i] = (gameMessagesPlayer[i].m_input) >> 8;
		temp = (gameMessagesPlayer[i].m_input) << 8;
		sendArray[5 + 4 * i] = temp >> 8;
	}

	sendArray[45] = BCMessage(sendArray).m_messageID;
	socketUDP.Send(serverAddress, (char*)sendArray, 46);
}

void NetworkSystem::RoomRequestAnswer(char* p_receiveArray)
{
	if ((bool)p_receiveArray[2])
	{
		for (int i = 0; (i < 20); i++)
		{
			opponentName[i] = p_receiveArray[i + 5];
			UE_LOG(LogTemp, Warning, TEXT("%c"), opponentName[i]);
		}

		myRoomID = p_receiveArray[3];
		clientID = p_receiveArray[4];

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
	for (int i = 0; (i < 20); i++)
	{
		opponentName[i] = p_receiveArray[i + 5];
		UE_LOG(LogTemp, Warning, TEXT("Letters %c"), p_receiveArray[i + 5]);
	}
	UMyUserWidget::myUserWidget->RivalJoinMessage(FString(UTF8_TO_TCHAR(opponentName)));
	UE_LOG(LogTemp, Warning, TEXT("RivalJoinMessage"));
}
void NetworkSystem::CreateRoomAnswer(char* p_receiveArray)
{
	if ((bool)p_receiveArray[2])
	{
		myRoomID = p_receiveArray[3];
		clientID = p_receiveArray[4];
		roomOwner = true;
	}
	else
	{
		//failed room creation
	}
	if (UMyUserWidget::myUserWidget != NULL)
	{
		UMyUserWidget::myUserWidget->CreateRoomMessage((bool)status);
	}
}
void NetworkSystem::LeaveRoomAnswer(char* p_receiveArray)
{
	if (UMyUserWidget::myUserWidget != NULL)
	{
		UMyUserWidget::myUserWidget->LeaveRoomMessage((bool)p_receiveArray[2]);
	}
}
void NetworkSystem::OpponentLeftRoom(char* p_receiveArray)
{
	sendArray[0] = identifier;
	UMyUserWidget::myUserWidget->RivalLeaveMessage();
}
void NetworkSystem::ElementUpdate(char* p_receiveArray)
{
	UMyUserWidget::myUserWidget->UpdateLobbyValues((int)p_receiveArray[3], (int)p_receiveArray[4], (bool)p_receiveArray[5]);
}
void NetworkSystem::PauseGameUpdate(char* p_receiveArray)
{
}
void NetworkSystem::OppentGameMessage(char* p_receiveArray)
{
	unsigned short inputVal;
	unsigned short timeVal = p_receiveArray[2];

	timeVal |= p_receiveArray[3] >> 8;

	for (int i = 0; i < 9; ++i)
	{
		timeVal = static_cast<unsigned int>(static_cast<unsigned char>(p_receiveArray[2 + 4 * i])) << 8;
		timeVal |= static_cast<unsigned int>(static_cast<unsigned char>(p_receiveArray[3 + 4 * i]));

		inputVal = static_cast<unsigned int>(static_cast<unsigned char>(p_receiveArray[4 + 4 * i])) << 8;
		inputVal |= static_cast<unsigned int>(static_cast<unsigned char>(p_receiveArray[5 + 4 * i]));


		if (timeVal - 1 == gameMessagesRivale[i].m_time)
		{
			for (; i > -1; --i)
			{
				gameMessagesRivale.insert(gameMessagesRivale.begin(), GameMessageData(timeVal, inputVal));
			}
			break;
		}
	}

	gameMessagesRivale.resize(9);

}
void NetworkSystem::StartGame()
{
	UMyUserWidget::myUserWidget->StartGame();
}
