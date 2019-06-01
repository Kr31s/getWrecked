#include "ServerDefines.h"

#include "BCServer.h"
#include "BCRoom.h"
#include "BCClient.h"
#include "BCMessage.h"

void DecodeMessageServer(NetAddress& receiveAddress, char* receiveArray, unsigned char& rounds, unsigned char& gameTime, unsigned char& identifier, unsigned char& status);
void ClearReceiveArray(char* receiveArray, long long length)
{
	for (int i = 0; (receiveArray[i] != NULL) && i < length; ++i)
	{
		receiveArray[i] = NULL;
	}
}


void ServerThread()
{
	NetAddress receiveAddress;
	char receiveArray[51] = { 0 };

	unsigned char rounds = 0;
	unsigned char gameTime = 0;
	unsigned char identifier = NULL;
	unsigned char status = NULL;


	while (BCServer::theServer->serverRunning)
	{
		receiveAddress = BCServer::theServer->serverSocket.Receive((char*)receiveArray, sizeof(receiveArray));
		if (receiveAddress.GetPortRef() != NULL)
		{
			DecodeMessageServer(receiveAddress, receiveArray, rounds, gameTime, identifier, status);
			ClearReceiveArray(receiveArray, sizeof(receiveArray));
		}
	}
	Println("ServerThread closed");
}
void MessageThread()
{
	while (BCServer::theServer->serverRunning)
	{
		//BCMessage::CheckResendMessages();
	}
	Println("MessageThread closed");
}
void HeartThread()
{
	char heartThreadArray[2] = { 0 };
	heartThreadArray[0] = 5 << 1;

	while (BCServer::theServer->serverRunning)
	{
		for (int i = 0; i < BCClient::totalClientID; ++i)
		{
			BCServer::theServer->SendDataBCM(&BCServer::theServer->clientIDList->at(i), False, heartThreadArray, 1);
		}

		//Println("Sleep 2sec");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	Println("HeartThread closed");
}

void NonServerMessage()
{

}

void DecodeMessageServer(NetAddress& receiveAddress, char* receiveArray, unsigned char& rounds, unsigned char& gameTime, unsigned char& identifier, unsigned char& status)
{
	identifier = receiveArray[0] >> 1;
	status = receiveArray[0] << 7;
	status = status >> 7;

	if (identifier == 5)
		BCServer::theServer->HeartBeat(receiveAddress, receiveArray);

	if (status == 0)
	{
		switch (identifier)
		{
		case 0:
			BCServer::theServer->RoomRequest(receiveAddress, receiveArray, rounds, gameTime);
			break;
		case 2:
			BCServer::theServer->CreateRoom(receiveAddress, receiveArray, rounds, gameTime);
			break;
		case 3:
			BCServer::theServer->LeaveRoom(receiveAddress, receiveArray, rounds, gameTime);
			break;
		case 6:
			BCServer::theServer->LobbyValueChange(receiveAddress, receiveArray);
			break;
		default:
			NonServerMessage();
			break;
		}
	}
	else
	{
		BCMessage::GetReplyMessage(identifier);
	}
}

int main()
{
	BCServer::theServer = new BCServer(4405, true);

	std::thread t1(ServerThread);
	std::thread t2(MessageThread);
	std::thread t3(HeartThread);


	int i;
	std::cin >> i;

	BCServer::theServer->serverRunning = false;

	t1.join();
	t2.join();
	t3.join();

	return 0;
}
