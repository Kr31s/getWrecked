#include "NetworkSystem.h"

NetworkSystem* NetworkSystem::NetSys = NULL;

bool NetworkSystem::StartingMessageReceiveThread()
{
	MessageReceiveThread = FMessageReceiveThread::InitThread(&socketUDP, m_receiveArray);

	return false;
}

NetworkSystem::NetworkSystem()
{

}


NetworkSystem::~NetworkSystem()
{
}

void NetworkSystem::TaskMessageReceiveThread(char* receivearray)
{
}

bool NetworkSystem::InitNetSystem()
{
	this->serverAddress = NetAddress(127, 0, 0, 1, 4405);

	BWNet::InitializeSocketLayer();

	if (socketUDP.OpenSocket(0).m_errorCode == 0)
	{
		if (socketUDP.EnableNonBlocking().m_errorCode == 0)
		{
			this->StartingMessageReceiveThread();
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void NetworkSystem::ClearReceiveArray()
{
	int a = sizeof(sendArray) / sizeof(*sendArray);
	for (int i = 0; i < a; ++i)
	{
		m_receiveArray[i] = 0;
		sendArray[i] = 0;
	}
}
