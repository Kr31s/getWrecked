#include "ServerDefines.h"

#include "BCServer.h"
#include "BCRoom.h"
#include "BCClient.h"
#include "BCMessage.h"

void DecodeMessageServer(NetAddress& p_receiveAddress, char* p_receiveArray, unsigned char& p_rounds, unsigned char& p_gameTime, Messages& p_identifier, unsigned char& p_status, unsigned int& p_intValue);
void ClearReceiveArray(char* p_receiveArray, long long p_length)
{
	for (int i = 0; (p_receiveArray[i] != NULL) && i < p_length; ++i)
	{
		p_receiveArray[i] = NULL;
	}
}


void ServerThread()
{
	NetAddress receiveAddress;
	char	receiveArray[46]	= { 0 };

	Messages		identifier	= UnknownMessage;
	unsigned int	intValue	= NULL;
	unsigned char	rounds		= NULL;
	unsigned char	gameTime	= NULL;
	unsigned char	status		= NULL;


	while (*BCServer::sTheServer->m_serverRunning)
	{
		receiveAddress = BCServer::sTheServer->m_serverSocket.Receive((char*)receiveArray, sizeof(receiveArray));
		if (receiveAddress.GetPortRef() != NULL)
		{
			DecodeMessageServer(receiveAddress, receiveArray, rounds, gameTime, identifier, status, intValue);

			ClearReceiveArray(receiveArray, sizeof(receiveArray));
			intValue	= NULL;
			rounds		= NULL;
			gameTime	= NULL;
			identifier	= UnknownMessage;
			status		= NULL;
		}
	}

	Println("ServerThread closed");
}
void MessageThread()
{
	while (*BCServer::sTheServer->m_serverRunning)
	{
		BCMessage::CheckResendMessages();
	}
	Println("MessageThread closed");
}
void HeartThread()
{
	char heartThreadArray[2] = { 0 };
	heartThreadArray[0] = 5 << 1;

	while (*BCServer::sTheServer->m_serverRunning)
	{
		for (int i = 0; i < BCServer::sTheServer->m_clientIDList->size(); ++i)
		{
			BCServer::sTheServer->SendDataBCM(BCServer::sTheServer->m_clientIDList->at(i).m_clientID, False, heartThreadArray);
		}

		Println("Sleep 2sec");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	Println("HeartThread closed");
}

void NonServerMessage()
{

}

void DecodeMessageServer(NetAddress& p_receiveAddress, char* p_receiveArray, unsigned char& p_rounds, unsigned char& p_gameTime, Messages& p_identifier, unsigned char& p_status, unsigned int& p_intValue)
{
	if (MessageOfIndex(p_receiveArray[0]) == 0)
	{
		switch (p_identifier)
		{
		case RoomRequest:
			BCServer::sTheServer->RoomRequest(p_receiveAddress, p_receiveArray, p_rounds, p_gameTime);
			break;
		case CreateRoom:
			BCServer::sTheServer->CreateRoom(p_receiveAddress, p_receiveArray, p_rounds, p_gameTime);
			break;
		case LeaveRoom:
			BCServer::sTheServer->LeaveRoom(p_receiveAddress, p_receiveArray);
			break;
		case HeartBeat:
			BCServer::sTheServer->HeartBeat(p_receiveAddress, p_receiveArray);
			break;
		case ElementChange:
			BCServer::sTheServer->ElementChange(p_receiveAddress, p_receiveArray);
			break;
		case PauseGame:
			BCServer::sTheServer->PauseGame(p_receiveAddress, p_receiveArray);
			break;
		case GameMessage:
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
