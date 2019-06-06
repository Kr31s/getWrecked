#include "BCClient.h"
#include "BCServer.h"
#include "BCRoom.h"
#include "BCMessage.h"

unsigned int BCClient::totalClientID = 0;
unsigned int BCClient::maxLeftHeartBeats = 5;

BCClient::BCClient(NetAddress p_netaddress, char* p_nickname)
{
	BCServer::theServer->clientIDList->size();
	m_netaddress = p_netaddress;

	for (int i = 2; (p_nickname[i] != NULL) && (i < 22); ++i)
	{
		m_nickname[i - 2] = p_nickname[i];
	}

	for (int i = 0; i <= BCClient::totalClientID; ++i)
	{
		if (BCServer::theServer->clientIDList->find(i) == BCServer::theServer->clientIDList->end())
		{
			m_clientID = i;

			if (i == BCClient::totalClientID)
			{
				++BCClient::totalClientID;
			}
			break;
		}
	}
	BCServer::theServer->clientIDList->insert({ m_clientID, *this });
}

void BCClient::resetHeartBeats()
{
	leftHeartBeats = 0;
}

bool BCClient::lostHeartBeat()
{
	++leftHeartBeats;
	if (leftHeartBeats >= maxLeftHeartBeats)
	{
		char arraytToSend[1];
		myRoom->RemoveRival(m_netaddress, arraytToSend);
		Println("clientIDList->size(): "<< BCServer::theServer->clientIDList->size());
		Println("roomIDList->size()" << BCServer::theServer->roomIDList->size());
		return false;
	}
	return true;
}
