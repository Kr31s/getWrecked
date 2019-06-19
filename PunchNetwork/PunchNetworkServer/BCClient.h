#pragma once
#include "ServerDefines.h"


class BCClient
{
public:
	static unsigned int		sTotalClientID;
	static unsigned int		sMaxLeftHeartBeats;

	unsigned int			m_lastClientFrame = 0;

	BCRoom*					m_myRoom = nullptr;
	NetAddress				m_netaddress;

	char					m_nickname[20];
	unsigned short			m_clientID;
	unsigned int			m_leftHeartBeats = 0;
	unsigned char			m_ping = 1500;

	BCClient				(NetAddress netaddress, char* nickname);
	void resetHeartBeats	();
	bool lostHeartBeat		(char* receiveArray);
};

