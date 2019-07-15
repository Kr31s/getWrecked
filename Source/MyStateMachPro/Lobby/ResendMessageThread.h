#pragma once
#include "AllowWindowsPlatformTypes.h"
#include "HideWindowsPlatformTypes.h"

#include "../ThirdParty/BWLib/include/Inc_BWNetworking.h"

#include "CoreMinimal.h"
#include "Runnable.h"
#include "RunnableThread.h"

class MYSTATEMACHPRO_API FResendMessageThread : public FRunnable
{
	NetSocketUDP* m_clientSocket;
	char* m_receiveArray;

public:
	static  FResendMessageThread* Runnable;

	static bool threadRuning;

	FRunnableThread* Thread;

	FResendMessageThread(NetSocketUDP* clientSocket, char* m_receiveArray);
	virtual ~FResendMessageThread();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	void EnsureCompletion();

	static FResendMessageThread* InitThread(NetSocketUDP* clientSocket, char* m_receiveArray);
	static void Shutdown();
};
