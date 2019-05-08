#pragma once
#include "BCRoom.h"
#include "BCClient.h"
#include "BCMessage.h"

class BCServer
{
public:

	static std::vector<BCRoom*> roomList[3][3];
	static std::map<unsigned int, BCRoom> roomIDList;
	static std::vector<BCMessage> messageListe;

	BCServer();
	~BCServer();

	static void SendData(NetAddress& netAddress, SendType p_status, char* dataArray, unsigned int lengthArrayToSend);
};

