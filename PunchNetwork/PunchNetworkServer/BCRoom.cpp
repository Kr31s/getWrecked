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
	//add new room
	for (int i = 0; i <= BCRoom::totalRoomID; ++i)
	{
		if (BCServer::theServer->roomIDList->find(i) == BCServer::theServer->roomIDList->end())
		{
			m_roomID = i;
			BCServer::theServer->roomIDList->insert({ i, *this });
			p_client->myRoom = &BCServer::theServer->roomIDList->at(i);
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

	if (this->m_Owner->m_netaddress == netAddress)
	{
		if (!m_full)
		{
			BCServer::theServer->deleteClient(this->m_Owner, p_receiveArray);
			BCServer::theServer->deleteRoom(this, p_receiveArray);
			return;
		}
		else
		{
			if (m_Owner != nullptr)
				BCServer::theServer->SendData(m_Owner->m_netaddress, True, p_receiveArray, 1);
			p_receiveArray[0] = 4 << 1;
			if (m_Member != nullptr)
				BCServer::theServer->SendDataBCM(m_Member->m_clientID, True, p_receiveArray, 1);

			BCServer::theServer->deleteClient(this->m_Owner, p_receiveArray);
			m_Owner = m_Member;
			m_Member = nullptr;
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
			Println("ERROR: Menber exist and room is not full")
			BCServer::theServer->deleteClient(this->m_Member, p_receiveArray);
			BCServer::theServer->deleteRoom(this, p_receiveArray);
			return;
		}
		else
		{
			if (m_Member != nullptr)
				BCServer::theServer->SendData(m_Member->m_netaddress, True, p_receiveArray, 1);
			p_receiveArray[0] = 4 << 1;
			if (m_Owner != nullptr)
				BCServer::theServer->SendDataBCM(m_Owner->m_clientID, True, p_receiveArray, 1);

			BCServer::theServer->deleteClient(this->m_Member, p_receiveArray);
			m_full = false;
			Print("One player left the room with the ID " << m_roomID);
			return;
		}
		if (m_Member != nullptr)
			BCServer::theServer->SendData(m_Member->m_netaddress, False, p_receiveArray, 1);
	}
}

bool BCRoom::FindClient(NetAddress & p_netAddress)
{
	if (((m_Owner == nullptr) ? false : p_netAddress == m_Owner->m_netaddress) || ((m_Member == nullptr) ? false : p_netAddress == m_Member->m_netaddress))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool BCRoom::IsOwner(NetAddress & p_netAddress)
{
	if (p_netAddress == m_Owner->m_netaddress)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BCClient* BCRoom::GetClient(NetAddress & p_netAddress)
{
	if (m_Owner->m_netaddress == p_netAddress)
	{
		return m_Owner;
	}
	else if (m_Member->m_netaddress == p_netAddress)
	{
		return m_Member;
	}
}

BCClient* BCRoom::GetRival(NetAddress & p_netAddress)
{

	if (m_Owner->m_netaddress == p_netAddress)
	{
		return m_Member;
	}
	else if (m_Member->m_netaddress == p_netAddress)
	{
		return m_Owner;
	}

}
