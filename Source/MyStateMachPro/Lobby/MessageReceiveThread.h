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

	bool threadRuning = true;

	FRunnableThread* Thread;

	FMessageReceiveThread();
	virtual ~FMessageReceiveThread();

	virtual bool Init();
	virtual uint32 Run();

	void EnsureCompletion();

	void InitThread(NetSocketUDP* clientSocket, char* m_receiveArray);
	static void Shutdown();

	static bool IsThreadFinished();

private:
};
