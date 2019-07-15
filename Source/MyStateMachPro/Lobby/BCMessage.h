#pragma once


class BCMessage
{
public:
	static unsigned int sTotalMessageID;
	unsigned short m_messageID;
	bool m_finished = false;

	unsigned int m_clientID;
	long long m_timeStamp;

	char m_messageArray[46];

	BCMessage(char* messageArray);
	static void GetReplyMessage(unsigned int messageID);
};