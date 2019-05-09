#pragma once
#include "ServerDefines.h"

class BCRoom
{
public:
	static unsigned int totalRoomID;

	bool m_full = false;
	unsigned short m_roomID;

	unsigned short m_roundState;
	unsigned short m_timeState;

	BCClient* m_Owner = nullptr;
	BCClient* m_Member = nullptr;

	BCRoom(BCClient* client, unsigned short roundState, unsigned short timeState);
	void AddRival(BCClient* client);
};
