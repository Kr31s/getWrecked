#include "ServerDefines.h"

#include "BCServer.h"
#include "BCRoom.h"
#include "BCClient.h"
#include "BCMessage.h"


void DecodeMessageServer(NetAddress& p_receiveAddress, char* p_receiveArray, unsigned char& p_rounds, unsigned char& p_gameTime, unsigned int& p_intValue);



void ServerThread()
{
	NetAddress receiveAddress;
	char	serverThreadArray[46] = { 0 };

	Messages		identifier = Messages::UnknownMessage;
	unsigned int	intValue = NULL;
	unsigned char	rounds = NULL;
	unsigned char	gameTime = NULL;
	unsigned char	status = NULL;


	while (BCServer::sTheServer->m_serverRunning)
	{
		receiveAddress = BCServer::sTheServer->m_serverSocket.Receive((char*)serverThreadArray, sizeof(serverThreadArray));
		if (receiveAddress.GetPortRef() != NULL)
		{
			DecodeMessageServer(receiveAddress, serverThreadArray, rounds, gameTime, intValue);

			ClearReceiveArray(serverThreadArray, sizeof(serverThreadArray));
			intValue = NULL;
			rounds = NULL;
			gameTime = NULL;
			identifier = Messages::UnknownMessage;
			status = NULL;
		}
	}

	Println("ServerThread closed");
}
void MessageThread()
{
	char	messageThreadArray[46] = { 0 };

	while (BCServer::sTheServer->m_serverRunning)
	{
		BCMessage::CheckResendMessages(messageThreadArray);
	}
	Println("MessageThread closed");
}
void HeartThread()
{
	char heartThreadArray[46] = { 0 };
	heartThreadArray[0] = 5;

	while (BCServer::sTheServer->m_serverRunning)
	{
		for (int i = 0; i < BCServer::sTheServer->m_clientIDList->size(); ++i)
		{
			if (BCServer::sTheServer->m_clientIDList->at(i).m_clientStatus != ClientStatus::Offline)
			{
				BCServer::sTheServer->SendDataBCM(BCServer::sTheServer->m_clientIDList->at(i).m_clientID, SendType::False, heartThreadArray);
			}
		}

		Println("Sleep 2sec");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	Println("HeartThread closed");
}

void NonServerMessage()
{

}

void DecodeMessageServer(NetAddress& p_receiveAddress, char* p_receiveArray, unsigned char& p_rounds, unsigned char& p_gameTime, unsigned int& p_intValue)
{
	if (p_receiveArray[1] == 0)
	{
		switch (MessageOfIndex(p_receiveArray[0]))
		{
		case Messages::RoomRequest:
			BCServer::sTheServer->RoomRequest(p_receiveAddress, p_receiveArray, p_rounds, p_gameTime);
			break;
		case Messages::CreateRoom:
			BCServer::sTheServer->CreateRoom(p_receiveAddress, p_receiveArray, p_rounds, p_gameTime);
			break;
		case Messages::LeaveRoom:
			BCServer::sTheServer->LeaveRoom(p_receiveAddress, p_receiveArray);
			break;
		case Messages::ElementChange:
			BCServer::sTheServer->ElementChange(p_receiveAddress, p_receiveArray);
			break;
		case Messages::PauseGame:
			BCServer::sTheServer->PauseGame(p_receiveAddress, p_receiveArray);
			break;
		case Messages::GameMessage:
			BCServer::sTheServer->GameMessage(p_receiveAddress, p_receiveArray, p_intValue);
			break;


		default:
			NonServerMessage();
			break;
		}
	}
	else
	{
		BCMessage::GetReplyMessage((int)p_receiveArray[0]);
	}
}

int main()
{
	BCServer::sTheServer = new BCServer(4405, true);

	std::thread t1(ServerThread);
	std::thread t2(MessageThread);
	std::thread t3(HeartThread);

	int i;
	std::cin >> i;

	BCServer::sTheServer->m_serverRunning = false;

	t1.join();
	t2.join();
	t3.join();

	return 0;
}
