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

	bool threadRuning = true;

	FRunnableThread* Thread;

	FResendMessageThread();
	virtual ~FResendMessageThread();

	virtual bool Init();
	virtual uint32 Run();

	void EnsureCompletion();

	static void Shutdown();

	static bool IsThreadFinished();

private:
};
