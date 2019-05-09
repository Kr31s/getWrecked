#pragma once
#include "ServerDefines.h"


class BCClient
{
public:
	static unsigned int totalClientID;

	NetAddress m_netaddress;
	unsigned short m_clientID;

	char m_nickname[20];
	unsigned char m_ping = 150;

	BCClient(NetAddress netaddress, char* nickname);
};

