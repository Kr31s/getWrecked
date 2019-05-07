#include "BCClient.h"



BCClient::BCClient()
{
}


BCClient::BCClient(NetAddress p_netaddress, char* p_nickname)
{
	m_netaddress = p_netaddress;
	for (int i = 2; (p_nickname[i] != NULL) && (i < 22); ++i)
	{
		m_nickname[i - 2] = p_nickname[i];
	}
}
