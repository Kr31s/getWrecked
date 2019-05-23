#pragma once

#include "AllowWindowsPlatformTypes.h"
#include "HideWindowsPlatformTypes.h"

#include "../ThirdParty/BWLib/include/Inc_BWNetworking.h"

#include "CoreMinimal.h"

#include "MessageReceiveThread.h"
#include "ResendMessageThread.h"

class NetworkSystem
{
public:
	NetSocketUDP socketUDP;
	NetAddress serverAddress;


	char m_receiveArray[51];
	unsigned char sendArray[51];
	unsigned char heartBeatArray[51];


	unsigned char opponentName[20];
	int myRoomID = -1;
	unsigned char identifier = NULL;
	unsigned char status = NULL;



	FMessageReceiveThread MessageReceiveThread;
	//FResendMessageThread ResendMessageThread;

	static NetworkSystem* NetSys;
	
	bool StartingMessageReceiveThread();
	//bool StartingResendMessageThread();



	NetworkSystem();
	~NetworkSystem();

	void TaskMessageReceiveThread(char* receivearray);
	//void TaskResendMessageThread(char* receivearray);

	bool InitNetSystem();

	void ClearReceiveArray();
};

