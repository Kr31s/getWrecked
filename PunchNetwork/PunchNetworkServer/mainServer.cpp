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

void ReplyMessage(unsigned char& gameTime)
{
	if (theServer->messageIDList->find(gameTime) == theServer->messageIDList->end())
		Println("Reply message number unkown");

	theServer->messageIDList->erase(gameTime);
}
void CheckMessages()
{
	for (int i = 0; i < theServer->messageIDList->size(); ++i)
	{
		if (theServer->messageIDList->find(i) == theServer->messageIDList->end())
		{
			continue;
		}

		if (theServer->messageIDList->at(i).m_timeStamp < GetTimeInMilli() + theServer->messageIDList->at(i).m_receiver->m_ping + 20)
		{
			theServer->SendDataBCM(theServer->messageIDList->at(i).m_receiver, None, theServer->messageIDList->at(i).m_messageArray, theServer->messageIDList->at(i).m_messageArrayLength);
			theServer->messageIDList->erase(i);
		}
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

	while (theServer->serverRunning)
	{
		receiveAddress = theServer->serverSocket.Receive((char*)receiveArray, sizeof(receiveArray));
		if (receiveAddress.GetPortRef() != NULL)
		{
			DecodeMessageServer(receiveAddress, receiveArray, rounds, gameTime, identifier, status);
			ClearReceiveArray(receiveArray, sizeof(receiveArray));
		}
	}
}
void MessageThread()
{
	while (theServer->serverRunning)
	{
		CheckMessages();
	}
}
void HeartThread()
{
	char heartThreadArray[2] = { 0 };
	heartThreadArray[0] = 5 << 1;

	while (theServer->serverRunning)
	{
		for (int i = 0; i < BCClient::totalClientID; ++i)
		{
			theServer->SendDataBCM(&theServer->clientIDList->at(i), False, heartThreadArray, 1);
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void NonServerMessage()
{

}

void DecodeMessageServer(NetAddress & receiveAddress, char* receiveArray, unsigned char& rounds, unsigned char& gameTime, unsigned char& identifier, unsigned char& status)
{
	identifier = receiveArray[0] >> 1;
	status = receiveArray[0] << 7;
	status = status >> 7;

	if (identifier == 5)
		theServer->HeartBeat(receiveAddress, receiveArray);

	if (status == 0)
	{
		switch (identifier)
		{
		case 0:
			theServer->RoomRequest(receiveAddress, receiveArray, rounds, gameTime);
			break;
		case 2:
			theServer->CreateRoom(receiveAddress, receiveArray, rounds, gameTime);
			break;
		case 3:
			theServer->LeaveRoom(receiveAddress, receiveArray, rounds, gameTime);
			break;
		default:
			NonServerMessage();
			break;
		}
	}
	else
	{
		ReplyMessage(identifier);
	}
}

int main()
{
	theServer = new BCServer(4405, false);

	std::thread t1(ServerThread);
	std::thread t2(MessageThread);
	std::thread t3(HeartThread);

	int r;
	std::cin >> r;

	return 0;
}