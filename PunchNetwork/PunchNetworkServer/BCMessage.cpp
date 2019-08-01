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
		sMutexMessageIDList.lock();

		if (BCServer::sTheServer->m_messageIDList->find(i) == BCServer::sTheServer->m_messageIDList->end())
		{
			sMutexMessageIDList.unlock();
			continue;
		}
		if (BCServer::sTheServer->m_messageIDList->at(i).m_finished)
		{
			BCServer::sTheServer->m_messageIDList->erase(i);
			sMutexMessageIDList.unlock();
			continue;
		}
		if (BCServer::sTheServer->m_messageIDList->at(i).m_timeStamp + 150 < GetTimeInMilli())
		{
			Println("HearthAtk");
			if (BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(i).m_clientID).lostHeartBeat(p_receiveArray) && (int)BCServer::sTheServer->m_messageIDList->at(i).m_messageArray[0] != 5)
			{
				BCServer::sTheServer->SendData(BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(i).m_clientID).m_clientID, SendType::NeedAnswer, BCServer::sTheServer->m_messageIDList->at(i).m_messageArray);
			}
			BCServer::sTheServer->m_messageIDList->at(i).m_finished = true;
		}
		sMutexMessageIDList.unlock();

	}
}

void BCMessage::GetReplyMessage(unsigned int p_messageID)
{
	sMutexMessageIDList.lock();
	if (BCServer::sTheServer->m_messageIDList->find(p_messageID) == BCServer::sTheServer->m_messageIDList->end())
	{
		Println("Reply message number unkown");
		sMutexMessageIDList.unlock();
		return;
	}
	BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(p_messageID).m_clientID).resetHeartBeats();
	BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(p_messageID).m_clientID).m_ping = GetTimeInMilli() - BCServer::sTheServer->m_messageIDList->at(p_messageID).m_timeStamp;
	Println("New Ping: " << (int)BCServer::sTheServer->m_clientIDList->at(BCServer::sTheServer->m_messageIDList->at(p_messageID).m_clientID).m_ping);
	BCServer::sTheServer->m_messageIDList->at(p_messageID).m_finished = true;
	sMutexMessageIDList.unlock();
	Println("Reply message received");
}
