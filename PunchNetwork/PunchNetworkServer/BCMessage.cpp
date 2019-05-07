#include "BCMessage.h"



BCMessage::BCMessage(NetAddress& p_netAddress, long long p_timeStamp, char* p_messageArray, unsigned int p_messageArrayLength)
{
	m_netAddress = p_netAddress;
	m_timeStamp = p_timeStamp;

	for (int i = 0; i < m_messageArrayLength; ++i)
	{
		m_messageArray[i] = p_messageArray[i];
	}
}
