#pragma once
#include "ServerDefines.h"


class BCClient
{
public:
	NetAddress m_netaddress;
	char m_nickname[20];
	unsigned char m_ping;

	BCClient();
	BCClient(NetAddress netaddress, char* nickname);
};

