#pragma once
#include "Inc_BWNetworking.h"
#include "Inc_SmartMacros.h"
#include "Inc_BWMath.h"

#include <stdexcept>
#include <vector>
#include <thread>
#include <chrono>
#include <bitset>
#include <algorithm>

class PClient;
class PRoom;
class PMessage;

static std::vector<PRoom*> roomList[3][3];
static std::vector<PRoom> roomIDList;
static std::vector<PMessage> messageListe;
