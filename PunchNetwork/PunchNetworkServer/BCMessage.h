#pragma once
#include "ServerDefines.h"


class BCMessage
{
public:
	static unsigned int totalMessageID;
	unsigned short m_messageID;

	BCClient* m_receiver;
	long long m_timeStamp;

	char m_messageArray[50];
	unsigned int m_messageArrayLength;

	BCMessage(BCClient* receiver, long long timeStamp, char* messageArray, unsigned int messageArrayLength);
};