#pragma once
#include "ServerDefines.h"

class BCServer
{
public:
	bool*								serverRunning	= new bool(true);

	NetSocketUDP						serverSocket;

	std::map<unsigned int, BCRoom>*		roomIDList		= new std::map<unsigned int, BCRoom>();
	std::map<unsigned int, BCClient>*	clientIDList	= new std::map<unsigned int, BCClient>();
	std::map<unsigned int, BCMessage>*	messageIDList	= new std::map<unsigned int, BCMessage>();

	std::vector<BCRoom*>*				roomList		= new std::vector<BCRoom*>[3 * 3]; //roomList[i * sizeof(j) + j] insane

	BCServer(unsigned short port, bool enableNonBlocking);
	~BCServer();

	//BCClient needed to add it to controllMessage
	void SendDataBCM(BCClient* receiver, SendType p_status, char* dataArray, unsigned int lengthArrayToSend);
	//Only address needed, no controllMessage
	void SendData(NetAddress& netAddress, SendType p_status, char* dataArray, unsigned int lengthArrayToSend);


	void RoomRequest(NetAddress& receiveAddress, char* receiveArray, unsigned short rounds, unsigned short gameTime);
	void CreateRoom(NetAddress& receiveAddress, char* receiveArray, unsigned short rounds, unsigned short gameTime);
	void LeaveRoom(NetAddress& receiveAddress, char* receiveArray, unsigned short rounds, unsigned short gameTime);
	void HeartBeat(NetAddress& receiveAddress, char* receiveArray);
};

