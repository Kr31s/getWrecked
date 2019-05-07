#pragma once
#include "ServerDefines.h"


class BCClient
{
public:
	NetAddress m_netaddress;
	char m_nickname[20];

	BCClient();
	BCClient(NetAddress p_netaddress, char* p_nickname);
};

