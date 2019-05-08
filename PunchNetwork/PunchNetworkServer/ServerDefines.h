#pragma once
#include "Inc_BWNetworking.h"
#include "Inc_SmartMacros.h"
#include "Inc_BWMath.h"

#include <vector>
#include <thread>
#include <chrono>
#include <map>

static unsigned int totalRoomID = 0;
static bool serverRunning = true;


enum SendType
{
	None,
	True,
	False
};
