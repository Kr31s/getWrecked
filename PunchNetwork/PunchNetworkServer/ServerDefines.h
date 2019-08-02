#pragma once
#include "Inc_BWNetworking.h"
#include "Inc_SmartMacros.h"
#include "Inc_BWMath.h"

#include <vector>
#include <thread>
#include <chrono>
#include <map>
#include <mutex>


#define DebugModus false;

class BCServer;
class BCRoom;
class BCClient;
class BCMessage;


static std::mutex sMutexClientIDList;
static std::mutex sMutexMessageIDList;


enum class SendType
{
	NoAnswer,
	NeedAnswer,
	Answer
};

enum class Messages
{
	UnknownMessage = -1,
	RoomRequest = 0,
	CreateRoom = 2,
	LeaveRoom = 3,
	ElementChange = 6,
	PauseGame = 8,
	GameMessage = 10
};

static Messages MessageOfIndex(int i) { return static_cast<Messages>(i); }


static long long GetTimeInMilli()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

static void ErrorCheck(short errorCode)
{
	if (errorCode != 0)
		Println("Error: " << errorCode);
}

static void CharArrayAddChar(char* firstArray, unsigned int startPosFirstArray, char* secondArray, unsigned int startPosSecondArray, unsigned int arrayLength)
{
	for (unsigned int i = 0; i < arrayLength; ++i)
	{
		firstArray[startPosFirstArray + i] = secondArray[startPosSecondArray + i];
	}
}

static void ClearReceiveArray(char* p_receiveArray, long long p_length)
{
	for (int i = 0; i < p_length; ++i)
	{
		p_receiveArray[i] = NULL;
	}
}