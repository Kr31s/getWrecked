#pragma once

#include "AllowWindowsPlatformTypes.h"
#include "HideWindowsPlatformTypes.h"

#include "../ThirdParty/BWLib/include/Inc_BWNetworking.h"

#include "CoreMinimal.h"

#include "MessageReceiveThread.h"
#include "ResendMessageThread.h"
#include "../../../Plugins/StateMachine/Source/StateMachine/Classes/SM_State.h"
#include "MyStateMachProGameModeBase.h"
#include <bitset>
#include <map>

class NetworkSystem
{
public:
	static NetworkSystem* NetSys;


	AMyStateMachProGameModeBase* gameMode;
	NetSocketUDP socketUDP;
	NetAddress serverAddress;
	FMessageReceiveThread* MessageReceiveThread;
	//FResendMessageThread* ResendMessageThread;

	char m_receiveArray[46];
	char sendArray[46];
	char heartBeatArray[46];
	std::map<unsigned int, unsigned short> receivedInputs;

	unsigned char opponentName[20];
	int myRoomID = -1;
	int clientID = -1;
	unsigned char identifier = NULL;
	unsigned char status = NULL;
	unsigned short frameValue = 0;
	unsigned short inputValue = 0;

	bool roomOwner;


	bool StartingMessageReceiveThread();
	//bool StartingResendMessageThread();

	NetworkSystem(AMyStateMachProGameModeBase* gameMode);
	~NetworkSystem();

	void TaskMessageReceiveThread(char* receivearray);
	//void TaskResendMessageThread(char* receivearray);

	bool InitNetSystem();

	void SendReceiveMessageClient();

	void ClearReceiveArray();

	//MessagesToSend
	void RoomRequest(int& timeValue, int& roundValue, const FString& name);
	void CreateRoom(int& timeValue, int& roundValue, const FString& name);
	void LeaveRoom();
	void ElementChanged(int& Element1_ID, int& Element2_ID, bool& ready);
	void PauseGame(bool& stop);
	void GameMessage(std::bitset<12>& inputStream);

	//MessagesToReceive
	void RoomRequestAnswer(unsigned char& status, char* receiveArray);
	void RoomJoin(char* receiveArray);
	void CreateRoomAnswer(unsigned char& status, char* receiveArray);
	void LeaveRoomAnswer(unsigned char& status, char* p_receiveArray);
	void OpponentLeftRoom(char* receiveArray);
	void Hearthbeat(char* receiveArray);
	void ElementUpdate(char* receiveArray);
	void PauseGameUpdate(unsigned char& status, char* receiveArray);
	void OppentGameMessage(char* receiveArray);
};

