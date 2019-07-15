#include "BCMessage.h"
#include "NetworkSystem.h"

unsigned int BCMessage::sTotalMessageID = 0;

BCMessage::BCMessage(char* p_messageArray)
{
	m_timeStamp = GetTimeInMilli();

	for (int i = 0; i < 46; ++i)
	{
		m_messageArray[i] = p_messageArray[i];
	}

	NetworkSystem::NetSys->sMutexMessageList.lock();
	for (unsigned int i = 0; i <= BCMessage::sTotalMessageID; ++i)
	{
		if (NetworkSystem::NetSys->m_messageIDList.find(i) == NetworkSystem::NetSys->m_messageIDList.end())
		{
			m_messageID = i;
			NetworkSystem::NetSys->m_messageIDList.insert({ i, *this });

			if (i == BCMessage::sTotalMessageID)
			{
				++BCMessage::sTotalMessageID;
			}
			break;
		}
	}
	NetworkSystem::NetSys->sMutexMessageList.unlock();
}


void BCMessage::GetReplyMessage(unsigned int p_messageID)
{
	if (NetworkSystem::NetSys->m_messageIDList.find(p_messageID) == NetworkSystem::NetSys->m_messageIDList.end())
		return;

	NetworkSystem::NetSys->sMutexMessageList.lock();
	NetworkSystem::NetSys->m_messageIDList.at(p_messageID).m_finished = true;
	NetworkSystem::NetSys->sMutexMessageList.unlock();
}
