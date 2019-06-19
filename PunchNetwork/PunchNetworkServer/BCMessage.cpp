#include "BCMessage.h"
#include "BCClient.h"
#include "BCServer.h"

unsigned int BCMessage::sTotalMessageID = 0;

BCMessage::BCMessage(unsigned int p_clientID, long long p_timeStamp, char* p_messageArray)
{
	m_clientID = p_clientID;
	m_timeStamp = p_timeStamp;

	for (int i = 0; i < 46; ++i)
	{
		m_messageArray[i] = p_messageArray[i];
	}

	for (int i = 0; i <= BCMessage::sTotalMessageID; ++i)
	{
		if (BCServer::sTheServer->m_messageIDList->find(i) == BCServer::sTheServer->m_messageIDList->end())
		{
			m_messageID = i;
			BCServer::sTheServer->m_messageIDList->insert({ i, *this });

			if (i == BCMessage::sTotalMessageID)
			{
				++BCMessage::sTotalMessageID;
			}
			break;
		}
	}
}

void BCMessage::CheckResendMessages(char* p_receiveArray)
{
	for (int i = 0; i < BCMessage::sTotalMessageID; ++i)
	{
		if (BCServer::sTheServer->m_messageIDList->find(i) == BCServer::sTheServer->m_messageIDList->end())
		{
			continue;
		}
		if (BCServer::sTheServer->m_messageIDList->at(i).m_finished)
		{
			Println(BCServer::sTheServer->m_messageIDList->size());

			BCServer::sTheServer->m_messageIDList->erase(i);
			Println(BCServer::sTheServer->m_messageIDList->size());

			continue;
		}
		if ((GetTimeInMilli() - BCServer::sTheServer->m_messageIDList->at(i).m_timeStamp) > BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(i).m_clientID).m_ping + (unsigned char)20)
		{
			Println("HearthAtk");
			if (BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(i).m_clientID).lostHeartBeat(p_receiveArray))
			{
				BCServer::sTheServer->SendDataBCM(BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(i).m_clientID).m_clientID, None, BCServer::sTheServer->m_messageIDList->at(i).m_messageArray);
			}
			BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(i).m_clientID).m_ping = 2000;
			BCServer::sTheServer->m_messageIDList->at(i).m_finished = true;
		}
	}
}

void BCMessage::GetReplyMessage(unsigned int p_messageID)
{
	if (BCServer::sTheServer->m_messageIDList->find(p_messageID) == BCServer::sTheServer->m_messageIDList->end())
	{
		Println("Reply message number unkown");
		return;
	}
	BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(p_messageID).m_clientID).resetHeartBeats();
	BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(p_messageID).m_clientID).m_ping = GetTimeInMilli() - BCServer::sTheServer->m_messageIDList->at(p_messageID).m_timeStamp;
	Println("New Ping: " << (int)BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(p_messageID).m_clientID).m_ping);
	BCServer::sTheServer->m_messageIDList->at(p_messageID).m_finished = true;
	Println("Reply message received");
}
