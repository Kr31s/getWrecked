#pragma once
#include "ServerDefines.h"

class BCServer
{
public:
	static BCServer*					theServer;

	bool*								serverRunning	= new bool(true);

	NetSocketUDP						serverSocket;

	std::map<unsigned int, BCRoom>*		roomIDList		= new std::map<unsigned int, BCRoom>();
	std::map<unsigned int, BCClient>*	clientIDList	= new std::map<unsigned int, BCClient>();
	std::map<unsigned int, BCMessage>*	messageIDList	= new std::map<unsigned int, BCMessage>();

	std::vector<BCRoom*>				roomList[3 * 3]; //roomList[i * sizeof(j) + j] insane

	BCServer(unsigned short port, bool enableNonBlocking);
	~BCServer();

	void deleteClient(BCClient* client, char* p_receiveArray);
	void deleteRoom(BCRoom* room, char* p_receiveArray);
	void deleteMessage(BCMessage* message, char* p_receiveArray);

	//BCClient needed to add it to controllMessage
	void SendDataBCM(unsigned int p_clientID, SendType p_status, char* dataArray, unsigned int lengthArrayToSend);
	//Only address needed, no controllMessage
	void SendData(NetAddress& netAddress, SendType p_status, char* dataArray, unsigned int lengthArrayToSend);

	void HeartBeat(NetAddress& receiveAddress, char* receiveArray);

	void RoomRequest(NetAddress& receiveAddress, char* receiveArray, unsigned char& rounds, unsigned char& gameTime);
	void CreateRoom(NetAddress& receiveAddress, char* receiveArray, unsigned char& rounds, unsigned char& gameTime);
	void LeaveRoom(NetAddress& receiveAddress, char* receiveArray);
	void ElementChange(NetAddress& receiveAddress, char* p_receiveArray);
	void PauseGame(NetAddress& receiveAddress, char* p_receiveArray);
	void GameMessage(NetAddress& receiveAddress, char* p_receiveArray, unsigned int& intValue);
};

