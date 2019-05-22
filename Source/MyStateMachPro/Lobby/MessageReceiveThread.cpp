
#include "MessageReceiveThread.h"
#include "PlatformProcess.h"


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
bool FMessageReceiveThread::Init(NetSocketUDP* p_clientSocket, char* p_receiveArray)
{
	m_clientSocket = p_clientSocket;
	m_receiveArray = p_receiveArray;

	return true;
}

//Run
uint32 FMessageReceiveThread::Run(void(*pt2Function)())
{
	while (threadRuning)
	{
		if (m_clientSocket->Receive(m_receiveArray, 50).GetPortRef() != NULL)
		{
			pt2Function();
		}
	}
	return 0;
}


void FMessageReceiveThread::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
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
