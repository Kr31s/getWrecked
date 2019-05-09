#include "BCClient.h"
#include "BCServer.h"


unsigned int BCClient::totalClientID = 0;

BCClient::BCClient(NetAddress p_netaddress, char* p_nickname)
{
	m_netaddress = p_netaddress;

	for (int i = 2; (p_nickname[i] != NULL) && (i < 22); ++i)
	{
		m_nickname[i - 2] = p_nickname[i];
	}

	for (int i = 0; i <= BCClient::totalClientID; ++i)
	{
		if (theServer->clientIDList->find(i) == theServer->clientIDList->end())
		{
			theServer->clientIDList->insert({ i, *this });
			m_clientID = i;

			if (i == BCClient::totalClientID)
			{
				++BCClient::totalClientID;
			}
			break;
		}
	}
}
