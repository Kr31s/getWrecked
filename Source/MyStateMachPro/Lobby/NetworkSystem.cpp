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

void NetworkSystem::TaskMessageReceiveThread(char* receivearray)
{
	identifier = m_receiveArray[0] >> 1;
	status = m_receiveArray[0] << 7;
	status = status >> 7;

	switch (identifier)
	{
	case 0:
		if (status)
		{
			for (int i = 0; (i < 20) && (m_receiveArray[i + 2] != -52); i++)
			{
				opponentName[i] = m_receiveArray[i + 2];
			}

			myRoomID = m_receiveArray[1];
		}
		else
		{
			//failed room join
		}
		UMyUserWidget::myUserWidget->JoinRoomMessage((bool)status);
		break;

	case 1:
		sendArray[0] = m_receiveArray[21];
		for (int i = 0; (i < 20) && (m_receiveArray[i + 1] != -52); i++)
		{
			opponentName[i] = m_receiveArray[i + 1];
		}
		SendReceiveMessageClient();
		UMyUserWidget::myUserWidget->RivalJoinMessage();

		break;

	case 2:
		if (status)
		{
			myRoomID = m_receiveArray[1];
		}
		else
		{
			//failed room creation
		}
		UMyUserWidget::myUserWidget->CreateRoomMessage((bool)status);
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
		sendArray[0] = m_receiveArray[1];
		UMyUserWidget::myUserWidget->RivalLeaveMessage();

		SendReceiveMessageClient();
		break;


	case 5:
		heartBeatArray[0] = m_receiveArray[1];
		heartBeatArray[0] = heartBeatArray[0] << 1;
		heartBeatArray[0] |= static_cast<char>(1);
		UE_LOG(LogTemp, Warning, TEXT("heart beat"));

		socketUDP.Send(serverAddress, (char*)heartBeatArray, 1).m_errorCode;

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
