#pragma once
#include "ServerDefines.h"

class BCMessage
{
public:
	NetAddress m_netAddress;
	long long m_timeStamp;

	char m_messageArray[50];
	unsigned int m_messageArrayLength;

	BCMessage(NetAddress& p_netAddress, long long p_timeStamp, char* p_messageArray, unsigned int p_messageArrayLength);
};