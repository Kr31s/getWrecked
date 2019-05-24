#pragma once
#include "AllowWindowsPlatformTypes.h"
#include "HideWindowsPlatformTypes.h"

#include "../ThirdParty/BWLib/include/Inc_BWNetworking.h"

#include "CoreMinimal.h"
#include "Runnable.h"
#include "RunnableThread.h"

class MYSTATEMACHPRO_API FMessageReceiveThread : public FRunnable
{
	NetSocketUDP* m_clientSocket;
	char* m_receiveArray;

public:
	static  FMessageReceiveThread* Runnable;

	static bool threadRuning;

	FRunnableThread* Thread;

	FMessageReceiveThread(NetSocketUDP* clientSocket, char* m_receiveArray);
	virtual ~FMessageReceiveThread();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	void EnsureCompletion();

	static FMessageReceiveThread* InitThread(NetSocketUDP* clientSocket, char* m_receiveArray);
	static void Shutdown();
};
