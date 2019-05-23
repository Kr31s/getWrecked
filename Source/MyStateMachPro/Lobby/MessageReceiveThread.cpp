
#include "MessageReceiveThread.h"
#include "PlatformProcess.h"
#include "NetworkSystem.h"


FMessageReceiveThread* FMessageReceiveThread::Runnable = NULL;
//***********************************************************

FMessageReceiveThread::FMessageReceiveThread()
{
	//Link to where data should be stored
	Thread = FRunnableThread::Create(this, TEXT("FMessageReceiveThread"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FMessageReceiveThread::~FMessageReceiveThread()
{
	delete Thread;
	Thread = NULL;
}

//Init
bool FMessageReceiveThread::Init()
{
	m_clientSocket = m_clientSocket;

	return true;
}

//Run
uint32 FMessageReceiveThread::Run()
{
	FPlatformProcess::Sleep(0.03);

	while (threadRuning)
	{
		if (m_clientSocket->Receive(m_receiveArray, 50).GetPortRef() != NULL)
		{
			NetworkSystem::NetSys->TaskMessageReceiveThread(m_receiveArray);
		}
	}
	return 0;
}


void FMessageReceiveThread::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void FMessageReceiveThread::InitThread(NetSocketUDP* p_clientSocket, char* p_receiveArray)
{
	m_clientSocket = p_clientSocket;
	m_receiveArray = p_receiveArray;

}

void FMessageReceiveThread::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}

bool FMessageReceiveThread::IsThreadFinished()
{
	if (Runnable)
	{
		return Runnable->IsThreadFinished();
	}
	return true;
}
