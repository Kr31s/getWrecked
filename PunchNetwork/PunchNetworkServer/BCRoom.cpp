#include "BCRoom.h"
#include "BCClient.h"



BCRoom::BCRoom(BCClient* p_client, unsigned short p_roundState, unsigned short p_timeState)
{
	m_Owner = p_client;
	m_roundState = p_roundState;;
	m_timeState = p_timeState;;
	m_roomID = totalRoomID;
}

void BCRoom::AddRival(BCClient* p_client)
{

	m_Member = p_client;
	m_full = true;

}
