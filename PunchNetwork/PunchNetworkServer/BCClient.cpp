#include "BCClient.h"
#include "BCServer.h"
#include "BCRoom.h"
#include "BCMessage.h"

unsigned int BCClient::sTotalClientID = 0;
unsigned int BCClient::sMaxLeftHeartBeats = 5;

BCClient::BCClient(NetAddress p_netaddress, char* p_nickname)
{
	BCServer::sTheServer->m_clientIDList->size();
	m_netaddress = p_netaddress;

	for (int i = 0; i < 20; ++i)
	{
		m_nickname[i] = p_nickname[i + 4];
		Print(m_nickname[i]);
	}

	for (int i = 0; i <= BCClient::sTotalClientID; ++i)
	{
		if (BCServer::sTheServer->m_clientIDList->find(i) == BCServer::sTheServer->m_clientIDList->end())
		{
			m_clientID = i;

			if (i == BCClient::sTotalClientID)
			{
				++BCClient::sTotalClientID;
			}
			break;
		}
	}
	BCServer::sTheServer->m_clientIDList->insert({ m_clientID, *this });
}

void BCClient::resetHeartBeats()
{
	m_leftHeartBeats = 0;
}

bool BCClient::lostHeartBeat(char* p_receiveArray)
{
	++m_leftHeartBeats;
	if (m_leftHeartBeats >= sMaxLeftHeartBeats)
	{
		m_clientStatus = ClientStatus::Offline;
		m_myRoom->RemoveRival(m_netaddress, p_receiveArray);
		Println("clientIDList->size(): "<< BCServer::sTheServer->m_clientIDList->size());
		Println("roomIDList->size(): " << BCServer::sTheServer->m_roomIDList->size());
		return false;
	}
	return true;
}
