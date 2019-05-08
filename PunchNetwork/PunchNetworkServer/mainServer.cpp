#include "ServerDefines.h"

#include "BCServer.h"
#include "BCRoom.h"
#include "BCClient.h"
#include "BCMessage.h"


NetAddress receiveAddress;

static char receiveArray[50] = { 0 };





void DecodeMessageServer();
void ClearReceiveArray()
{
	for (int i = 0; (receiveArray[i] != NULL) && i < sizeof(receiveArray); ++i)
	{
		receiveArray[i] = NULL;
	}
}

void ReplyMessage(NetAddress& p_netAddress)
{
	for (int i = 0; i < theServer->messageListe->size(); ++i)
	{
		if (theServer->messageListe->at(i).m_receiver->m_netaddress.GetIpv4Ref() == p_netAddress.GetIpv4Ref()
			&& theServer->messageListe->at(i).m_receiver->m_netaddress.GetPortRef() == p_netAddress.GetPortRef())
		{
			theServer->messageListe->erase(theServer->messageListe->begin() + i);
		}
	}
}
void CheckMessages()
{
	for (int i = 0; i < theServer->messageListe->size(); ++i)
	{
		if (theServer->messageListe->at(i).m_timeStamp < GetTimeInMilli() + theServer->messageListe->at(i).m_receiver->m_ping + 20)
		{
			theServer->SendDataBCM(theServer->messageListe->at(i).m_receiver, None, theServer->messageListe->at(i).m_messageArray, theServer->messageListe->at(i).m_messageArrayLength);
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
void ServerThread()
{
	while (theServer->serverRunning)
	{
		receiveAddress = theServer->serverSocket.Receive((char*)receiveArray, 50);
		if (receiveAddress.GetPortRef() != NULL)
		{
			DecodeMessageServer();
			ClearReceiveArray();
		}
	}
}

void ReceiveArrayAddString(unsigned int startPos, char* nameArray, unsigned int arrayLength)
{
	for (unsigned int i = startPos; i < (startPos + arrayLength); ++i)
	{
		receiveArray[i] = nameArray[i - startPos];
	}
}


void NonServerMessage()
{

}

void DecodeMessageServer()
{
	identifier = receiveArray[0] >> 1;
	status = receiveArray[0] << 7;
	status = status >> 7;

	if (status == 0)
	{
		switch (identifier)
		{
		case 0:
			RoomRequest();
			break;
		case 2:
			CreateRoom();
			break;
		case 3:
			LeaveRoom();
			break;
		default:
			NonServerMessage();
			break;
		}
	}
	else
	{
		ReplyMessage(receiveAddress);
	}
}

void main()
{
	theServer = new BCServer(4405, false);

	std::thread t1(ServerThread);
	std::thread t2(MessageThread);


	int r;
	std::cin >> r;
}
