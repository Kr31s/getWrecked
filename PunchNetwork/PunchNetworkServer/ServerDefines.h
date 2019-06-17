#pragma once
#include "Inc_BWNetworking.h"
#include "Inc_SmartMacros.h"
#include "Inc_BWMath.h"

#include <vector>
#include <thread>
#include <chrono>
#include <map>

#define DebugModus false;

class BCServer;
class BCRoom;
class BCClient;
class BCMessage;

enum SendType
{
	None,
	True,
	False
};

enum Messages
{
	UnknownMessage = -1,
	RoomRequest = 0,
	CreateRoom = 2,
	LeaveRoom = 3,
	HeartBeat = 5,
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

static void CharArrayAddChar(char* receiveArray, unsigned int startPos, char* nameArray, unsigned int arrayLength)
{
	for (unsigned int i = startPos; i < (startPos + arrayLength); ++i)
	{
		receiveArray[i] = nameArray[i - startPos];
		Println("letter" << receiveArray[i])
	}
}