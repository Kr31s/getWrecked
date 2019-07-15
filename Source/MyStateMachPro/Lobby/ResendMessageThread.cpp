
#include "ResendMessageThread.h"
#include "PlatformProcess.h"
#include "NetworkSystem.h"
#include "BCMessage.h"

FResendMessageThread* FResendMessageThread::Runnable = NULL;
bool FResendMessageThread::threadRuning = true;
//***********************************************************

FResendMessageThread::FResendMessageThread(NetSocketUDP* p_clientSocket, char* p_receiveArray)
{
	m_clientSocket = p_clientSocket;
	m_receiveArray = p_receiveArray;
	FResendMessageThread::threadRuning = true;
	Thread = FRunnableThread::Create(this, TEXT("FMessageReceiveThread"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FResendMessageThread::~FResendMessageThread()
{
	delete Thread;
	Thread = NULL;
}

//Init
bool FResendMessageThread::Init()
{
	return true;
}

//Run
uint32 FResendMessageThread::Run()
{
	while (FResendMessageThread::threadRuning)
	{
		NetworkSystem::NetSys->TaskResendMessageThread();
		FPlatformProcess::Sleep(0.0083333);//120 FPS
	}
	return 0;
}


FResendMessageThread* FResendMessageThread::InitThread(NetSocketUDP* p_clientSocket, char* p_receiveArray)
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FResendMessageThread(p_clientSocket, p_receiveArray);
	}
	return Runnable;
}

void FResendMessageThread::Stop()
{
	this->threadRuning = false;
}


void FResendMessageThread::EnsureCompletion()
{
	UE_LOG(LogTemp, Warning, TEXT("stop"));
	Stop();
	Thread->WaitForCompletion();
}

void FResendMessageThread::Shutdown()
{
	if (Runnable)
	{
		UE_LOG(LogTemp, Warning, TEXT("shutdown"));
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;

	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Runnable not true"));

	}
}
