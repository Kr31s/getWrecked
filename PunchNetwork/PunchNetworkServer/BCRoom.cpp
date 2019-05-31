#include "BCRoom.h"
#include "BCClient.h"
#include "BCServer.h"
#include "ServerDefines.h"


unsigned int BCRoom::totalRoomID = 0;

BCRoom::BCRoom(BCClient* p_client, unsigned char p_roundState, unsigned char p_timeState)
{
	m_Owner = p_client;
	m_roundState = p_roundState;;
	m_timeState = p_timeState;;
	p_client->myRoom = this;
	//add new room
	for (int i = 0; i <= BCRoom::totalRoomID; ++i)
	{
		if (BCServer::theServer->roomIDList->find(i) == BCServer::theServer->roomIDList->end())
		{
			m_roomID = i;
			BCServer::theServer->roomIDList->insert({ i, *this });
			BCServer::theServer->roomList[p_roundState * 3 + p_timeState].push_back(&BCServer::theServer->roomIDList->at(i));

			if (i == BCRoom::totalRoomID)
			{
				++BCRoom::totalRoomID;
			}
			break;
		}
	}
}

void BCRoom::AddRival(BCClient* p_client)
{
	m_Member = p_client;
	p_client->myRoom = this;
	m_full = true;
}

void BCRoom::RemoveRival(NetAddress& netAddress, char* p_receiveArray)
{
	p_receiveArray[0] = 3 << 1;
	this;

	if (m_Owner->m_netaddress == netAddress)
	{
		if (m_full)
		{
			for (int i = 0; i < BCServer::theServer->roomList[(m_roundState-1) * 3 + (m_timeState-1)].size(); ++i)
			{
				if (BCServer::theServer->roomList[(m_roundState - 1) * 3 + (m_timeState - 1)].at(i)->m_roomID == m_roomID)
				{
					if(m_Owner != nullptr)
						BCServer::theServer->SendData(m_Owner->m_netaddress, True, p_receiveArray, 1);

					BCServer::theServer->roomList[(m_roundState - 1) * 3 + (m_timeState - 1)].erase(BCServer::theServer->roomList[(m_roundState - 1) * 3 + (m_timeState - 1)].begin() + i);

					Println("One player left the room with the ID " << m_roomID);
					Println("The room with the ID " << m_roomID << " was deleted");
					BCServer::theServer->roomIDList->erase(m_roomID);
					Println("printing after destroyed");
					return;
				}
			}
		}
		else
		{
			if (m_Owner != nullptr)
			BCServer::theServer->SendData(m_Owner->m_netaddress, True, p_receiveArray, 1);
			p_receiveArray[0] = 4 << 1;
			if (m_Member != nullptr)
			BCServer::theServer->SendDataBCM(m_Member, True, p_receiveArray, 1);

			m_Owner = m_Member;
			delete(m_Member);
			m_full = false;
			Println("One player left the room with the ID " << m_roomID);
		}
		if (m_Owner != nullptr)
			BCServer::theServer->SendData(m_Owner->m_netaddress, False, p_receiveArray, 1);
	}
	else if (m_Member->m_netaddress == netAddress)
	{
		if (!m_full)
		{
			for (int i = 0; i < BCServer::theServer->roomList[(m_roundState - 1) * 3 + (m_timeState - 1)].size(); ++i)
			{
				if (BCServer::theServer->roomList[(m_roundState - 1) * 3 + (m_timeState - 1)].at(i)->m_roomID == m_roomID)
				{
					if (m_Member != nullptr)
						BCServer::theServer->SendData(m_Member->m_netaddress, True, p_receiveArray, 1);

					BCServer::theServer->roomList[(m_roundState - 1) * 3 + (m_timeState - 1)].erase(BCServer::theServer->roomList[(m_roundState - 1) * 3 + (m_timeState - 1)].begin() + i);
					Println("One player left the room with the ID " << m_roomID);
					Println("The room with the ID " << m_roomID << " was deleted");
					BCServer::theServer->roomIDList->erase(m_roomID);
					Println("printing after destroyed");
					return;
				}
			}
		}
		else
		{
			if (m_Member != nullptr)
				BCServer::theServer->SendData(m_Member->m_netaddress, True, p_receiveArray, 1);
			p_receiveArray[0] = 4 << 1;
			if (m_Owner != nullptr)
				BCServer::theServer->SendDataBCM(m_Owner, True, p_receiveArray, 1);

			delete(m_Member);
			m_full = false;
			Print("One player left the room with the ID " << m_roomID);
			return;
		}
		if (m_Member != nullptr)
			BCServer::theServer->SendData(m_Member->m_netaddress, False, p_receiveArray, 1);
	}
}

bool BCRoom::FindClient(NetAddress& p_netAddress)
{
	Println((p_netAddress == m_Owner->m_netaddress));

	if(((m_Owner == nullptr) ? false : p_netAddress == m_Owner->m_netaddress) || ((m_Member == nullptr) ? false : p_netAddress == m_Member->m_netaddress))
	{
		return true;
	}
	else
	{
		return false;
	}
}