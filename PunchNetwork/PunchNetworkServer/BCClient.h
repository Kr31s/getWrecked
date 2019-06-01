#pragma once
#include "ServerDefines.h"


class BCClient
{
public:
	static unsigned int totalClientID;
	static unsigned int maxLeftHeartBeats;


	BCRoom* myRoom = nullptr;
	NetAddress m_netaddress;

	char m_nickname[20];
	unsigned short m_clientID;
	unsigned int leftHeartBeats = 0;
	unsigned char m_ping = 1500;

	BCClient(NetAddress netaddress, char* nickname);
	void resetHeartBeats();
	void lostHeartBeat();
};

