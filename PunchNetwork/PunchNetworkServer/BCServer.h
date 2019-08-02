#pragma once
#include "ServerDefines.h"

class BCServer
{
public:
	static BCServer*					sTheServer;

	bool*								m_serverRunning	= new bool(true);

	NetSocketUDP						m_serverSocket;

	std::map<unsigned int, BCRoom>*		m_roomIDList	= new std::map<unsigned int, BCRoom>();
	std::map<unsigned int, BCClient>*	m_clientIDList	= new std::map<unsigned int, BCClient>();
	std::map<unsigned int, BCMessage>*	m_messageIDList	= new std::map<unsigned int, BCMessage>();

	std::vector<BCRoom*>				m_roomList[3 * 3]; //roomList[i * sizeof(j) + j] insane

	BCServer(unsigned short port, bool enableNonBlocking);
	~BCServer();

	void deleteClient(unsigned short& p_clientID, char* receiveArray);
	void deleteRoom(BCRoom* room, char* receiveArray);

	//Only address needed, no controllMessage
	void SendData(unsigned int clientID, SendType status, char* dataArray);

	void RoomRequest(NetAddress& receiveAddress, char* receiveArray, unsigned char& rounds, unsigned char& gameTime);
	void CreateRoom(NetAddress& receiveAddress, char* receiveArray, unsigned char& rounds, unsigned char& gameTime);
	void LeaveRoom(NetAddress& receiveAddress, char* receiveArray);
	void ElementChange(NetAddress& receiveAddress, char* receiveArray);
	void PauseGame(NetAddress& receiveAddress, char* receiveArray);
	void GameMessage(NetAddress& receiveAddress, char* receiveArray, unsigned int& intValue1, int& intValue2);
};

