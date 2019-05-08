#include "BCMessage.h"
#include "BCClient.h"


BCMessage::BCMessage(BCClient& p_receiver, long long p_timeStamp, char* p_messageArray, unsigned int p_messageArrayLength)
{
	m_receiver = &p_receiver;
	m_timeStamp = p_timeStamp;
	m_messageArrayLength = p_messageArrayLength;

	for (int i = 0; i < m_messageArrayLength; ++i)
	{
		m_messageArray[i] = p_messageArray[i];
	}
}
