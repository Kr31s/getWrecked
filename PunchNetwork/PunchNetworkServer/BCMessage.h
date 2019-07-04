#pragma once
#include "ServerDefines.h"


class BCMessage
{
public:
	static unsigned int sTotalMessageID;
	unsigned short m_messageID;
	bool m_finished = false;

	unsigned int m_clientID;
	long long m_timeStamp;

	char m_messageArray[46];

	BCMessage(unsigned int clientID, long long timeStamp, char* messageArray);
	static void CheckResendMessages(char* receiveArray);
	static void GetReplyMessage(unsigned int messageID);
};