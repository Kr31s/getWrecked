#pragma once
#include "ServerDefines.h"
#include "BCClient.h"

class BCRoom
{
public:
	bool m_full = false;
	unsigned short m_roomID;

	unsigned short m_roundState;
	unsigned short m_timeState;

	BCClient m_Owner;
	BCClient m_Member;

	BCRoom();
	BCRoom(BCClient client, unsigned short roundState, unsigned short timeState);
	void AddRival(BCClient client);
};

