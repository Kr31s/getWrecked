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

static BCServer* theServer = nullptr;
static unsigned char rounds = 0;
static unsigned char gameTime = 0;
static unsigned char identifier = NULL;
static unsigned char status = NULL;


long long GetTimeInMilli()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void ErrorCheck(short errorCode)
{
	if (errorCode != 0)
		Println("Error: " << errorCode);
}