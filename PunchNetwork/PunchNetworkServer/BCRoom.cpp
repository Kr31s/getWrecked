#include "BCRoom.h"
#include "BCClient.h"
#include "BCServer.h"


unsigned int BCRoom::totalRoomID = 0;

BCRoom::BCRoom(BCClient* p_client, unsigned short p_roundState, unsigned short p_timeState)
{
	//add new room
	for (int i = 0; i <= BCRoom::totalRoomID; ++i)
	{
		if (theServer->roomIDList->find(i) == theServer->roomIDList->end())
		{
			theServer->roomIDList->insert({ i, *this });
			theServer->roomList[p_roundState * 3 + p_timeState].push_back(&theServer->roomIDList->at(i));
			m_roomID = i;

			if (i == BCRoom::totalRoomID)
			{
				++BCRoom::totalRoomID;
			}
			break;
		}
	}


	m_Owner = p_client;
	m_roundState = p_roundState;;
	m_timeState = p_timeState;;
}

void BCRoom::AddRival(BCClient* p_client)
{

	m_Member = p_client;
	m_full = true;

}
