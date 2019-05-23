
#include "ResendMessageThread.h"
#include "PlatformProcess.h"


FResendMessageThread* FResendMessageThread::Runnable = NULL;
//***********************************************************

FResendMessageThread::FResendMessageThread()
{
	//Link to where data should be stored
	Thread = FRunnableThread::Create(this, TEXT("FResendMessageThread"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
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
	FPlatformProcess::Sleep(0.03);


	return 0;
}


void FResendMessageThread::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void FResendMessageThread::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}

bool FResendMessageThread::IsThreadFinished()
{
	if (Runnable)
	{
		return Runnable->IsThreadFinished();
	}
	return true;
}
