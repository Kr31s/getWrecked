#pragma once
#include "ServerDefines.h"


class BCMessage
{
public:
	static unsigned int totalMessageID;
	unsigned short m_messageID;
	bool finished = false;

	unsigned int m_clientID;
	long long m_timeStamp;

	char m_messageArray[50];
	unsigned int m_messageArrayLength;

	BCMessage(unsigned int clientID, long long timeStamp, char* messageArray, unsigned int messageArrayLength);
	static void CheckResendMessages();
	static void GetReplyMessage(unsigned char& messageID);
};