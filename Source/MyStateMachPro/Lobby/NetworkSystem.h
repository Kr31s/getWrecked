#pragma once

#include "AllowWindowsPlatformTypes.h"
#include "HideWindowsPlatformTypes.h"

#include "../ThirdParty/BWLib/include/Inc_BWNetworking.h"

#include "CoreMinimal.h"

#include "MessageReceiveThread.h"
#include "ResendMessageThread.h"
#include "../../../Plugins/StateMachine/Source/StateMachine/Classes/SM_State.h"
#include <bitset>
#include <vector>
#include <map>
#include <mutex>
#include "BCMessage.h"

static long long GetTimeInMilli()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

struct GameMessageData
{
	unsigned short m_time = 666;
	unsigned short m_input = 666;

	GameMessageData() : m_time{ 0 }, m_input{ 0 }{};
	GameMessageData(unsigned short p_time, unsigned short p_input) : m_time{ p_time }, m_input{ p_input }{};
	GameMessageData(const GameMessageData& gmd) : m_time{ gmd.m_time }, m_input{ gmd.m_input }{};
};

class AMyStateMachProGameModeBase;

class NetworkSystem
{
public:


	static NetworkSystem* NetSys;
	std::mutex sMutexMessageList;
	std::map<unsigned int, BCMessage> m_messageIDList;

	NetAddress serverAddress;
	AMyStateMachProGameModeBase* m_gameMode;
	NetSocketUDP socketUDP;
	FMessageReceiveThread* MessageReceiveThread;
	FResendMessageThread* ResendMessageThread;

	char m_receiveArray[1000];
	char sendArray[1000];
	char heartBeatArray[46];

	int myRoomID = -1;
	int clientID = -1;
	unsigned char m_opponentName[20];//FString(UTF8_TO_TCHAR(opponentName))
	unsigned char identifier = NULL;
	unsigned char status = NULL;
	unsigned short frameValue = 0;
	unsigned short inputValue = 0;

	std::vector<GameMessageData> gameMessagesRivale;
	std::vector<GameMessageData> gameMessagesPlayer;

	bool roomOwner;


	bool StartingMessageReceiveThread();
	bool StartingResendMessageThread();

	NetworkSystem();
	~NetworkSystem();

	void setGameMode(AMyStateMachProGameModeBase* gameMode);

	void TaskMessageReceiveThread(char* receivearray);
	void TaskResendMessageThread();
	void GetAnswerMessage(short messageID);

	bool InitNetSystem();

	void SendReceiveMessageClient();

	void ClearReceiveArray();
	void ClearSendArray();
	void ShutdownNetwork();

	//MessagesToSend
	void RoomRequest(int& timeValue, int& roundValue, const FString& name);
	void CreateRoom(int& timeValue, int& roundValue, const FString& name);
	void LeaveRoom();
	void ElementChanged(int& Element1_ID, int& Element2_ID, bool& ready);
	void PauseGame(bool& stop);
	void GameMessage(std::bitset<12>& inputStream);

	//MessagesToReceive
	void RoomRequestAnswer(char* receiveArray);
	void RoomJoin(char* receiveArray);
	void CreateRoomAnswer(char* receiveArray);
	void LeaveRoomAnswer(char* p_receiveArray);
	void OpponentLeftRoom(char* receiveArray);
	void ElementUpdate(char* receiveArray);
	void PauseGameUpdate(char* receiveArray);
	void SyncGame(char* receiveArray);
	void OppentGameMessage(char* receiveArray);
	void StartGame();
	void NextRound();
};

