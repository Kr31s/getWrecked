#pragma once
#include "ServerDefines.h"


class PClient
{
public:
	NetSocketTCP m_clientTCP;
	NetAddress m_netaddress;
	char m_nickname[20];

	PClient() {};
	PClient(NetAddress p_netaddress, char* p_nickname)
	{
		m_netaddress = p_netaddress;
		for (int i = 2; (p_nickname[i] != NULL) && (i < 22); ++i)
		{
			m_nickname[i - 2] = p_nickname[i];
		}
	}
};

