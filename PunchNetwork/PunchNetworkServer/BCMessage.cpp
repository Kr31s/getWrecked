#include "BCMessage.h"
#include "BCClient.h"
#include "BCServer.h"

unsigned int BCMessage::totalMessageID = 0;

BCMessage::BCMessage(BCClient* p_receiver, long long p_timeStamp, char* p_messageArray, unsigned int p_messageArrayLength)
{
	m_receiver = p_receiver;
	m_timeStamp = p_timeStamp;
	m_messageArrayLength = p_messageArrayLength;

	for (int i = 0; i < m_messageArrayLength; ++i)
	{
		m_messageArray[i] = p_messageArray[i];
	}

	for (int i = 0; i <= BCMessage::totalMessageID; ++i)
	{
		if (BCServer::theServer->messageIDList->find(i) == BCServer::theServer->messageIDList->end())
		{
			BCServer::theServer->messageIDList->insert({ i, *this });
			m_messageID = i;

			if (i == BCMessage::totalMessageID)
			{
				++BCMessage::totalMessageID;
			}
			break;
		}
	}
}

void BCMessage::CheckResendMessages()
{
	for (int i = 0; i < BCServer::theServer->messageIDList->size(); ++i)
	{
		if (BCServer::theServer->messageIDList->find(i) == BCServer::theServer->messageIDList->end())
		{
			continue;
		}

		if(BCServer::theServer->messageIDList->at(i).finished)
		{
			BCServer::theServer->messageIDList->erase(i);
			continue;
		}

		if ((GetTimeInMilli() - BCServer::theServer->messageIDList->at(i).m_timeStamp) > (BCServer::theServer->messageIDList->at(i).m_receiver->m_ping + 20))
		{
			Println("HearthAtk");
			BCServer::theServer->messageIDList->at(i).m_receiver->lostHeartBeat();
			BCServer::theServer->SendDataBCM(BCServer::theServer->messageIDList->at(i).m_receiver, None, BCServer::theServer->messageIDList->at(i).m_messageArray, BCServer::theServer->messageIDList->at(i).m_messageArrayLength);
			BCServer::theServer->messageIDList->erase(i);
		}
	}
}

void BCMessage::GetReplyMessage(unsigned char& messageID)
{
	if (BCServer::theServer->messageIDList->find(messageID) == BCServer::theServer->messageIDList->end())
	{
		Println("Reply message number unkown");
		return;
	}
	BCServer::theServer->messageIDList->at(messageID).m_receiver->resetHeartBeats();
	BCServer::theServer->messageIDList->at(messageID).m_receiver->m_ping = (GetTimeInMilli() - BCServer::theServer->messageIDList->at(messageID).m_timeStamp);
	Println("New Ping: " << (int)BCServer::theServer->messageIDList->at(messageID).m_receiver->m_ping);
	BCServer::theServer->messageIDList->at(messageID).finished = true;
	Println("Reply message received");
}
