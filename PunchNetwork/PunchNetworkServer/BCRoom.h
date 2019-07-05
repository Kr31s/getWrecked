#pragma once
#include "ServerDefines.h"

class BCRoom
{
public:
	static unsigned int sTotalRoomID;

	bool m_full = false;
	unsigned short m_roomID;

	bool m_gamePaused = false;

	unsigned short m_roundState;
	unsigned short m_timeState;

	BCClient* m_Owner = nullptr;
	BCClient* m_Member = nullptr;

	BCRoom(BCClient* client, unsigned char roundState, unsigned char timeState);
	void AddRival(BCClient* client);
	void RemoveRival(NetAddress& netAddress, char* receiveArray);
	bool FindClient(NetAddress& netAddress);
	bool IsOwner(NetAddress& netAddress);
	BCClient* GetClient(NetAddress& netAddress);
	BCClient* GetRival(NetAddress& netAddress);
};
