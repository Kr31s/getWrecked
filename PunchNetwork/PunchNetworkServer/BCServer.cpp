#include "BCServer.h"
#include "BCRoom.h"
#include "BCClient.h"
#include "BCMessage.h"

BCServer* BCServer::theServer = nullptr;

BCServer::BCServer(unsigned short p_port, bool p_enableNonBlocking)
{
	Print("InitializeSocketLayer: ");
	Println(((BWNet::InitializeSocketLayer().m_errorCode == 0) ? "Succes" : "Failed"));
	Print("OpenSocket 4405: ");
	Println(((serverSocket.OpenSocket(p_port).m_errorCode == 0) ? "Succes" : "Failed"));
	Print("Is Socket 4405 open ? ");
	Println(((serverSocket.IsOpen() == 1) ? "true" : "false"));

	if (p_enableNonBlocking)
	{
		Print("Enable Non-Blocking: ");
		Println(((serverSocket.EnableNonBlocking().m_errorCode == 0) ? "Succes" : "Failed"));
	}
	Println("");
}
BCServer::~BCServer()
{
	delete(roomIDList);
	delete(clientIDList);
	delete(messageIDList);
	delete[](roomList);
	delete(serverRunning);
}

void BCServer::SendDataBCM(BCClient * receiver, SendType p_status, char* dataArray, unsigned int lengthArrayToSend)
{
	dataArray[lengthArrayToSend] = BCMessage(receiver, GetTimeInMilli(), dataArray, lengthArrayToSend).m_messageID;

	switch (p_status)
	{
	case None:
		break;

	case True:
		dataArray[0] = dataArray[0] >> 1;
		dataArray[0] = (dataArray[0] << 1);
		dataArray[0] |= 1;
		break;

	case False:
		dataArray[0] = dataArray[0] >> 1;
		dataArray[0] = (dataArray[0] << 1);
		break;
	}

#if DebugModus == true
	ErrorCheck(serverSocket.Send(receiver->m_netaddress, (char*)dataArray, lengthArrayToSend + 1).m_errorCode);
#else
	serverSocket.Send(receiver->m_netaddress, (char*)dataArray, lengthArrayToSend + 1);
#endif
}

void BCServer::SendData(NetAddress& netAddress, SendType p_status, char* dataArray, unsigned int lengthArrayToSend)
{
	switch (p_status)
	{
	case None:
		break;

	case True:
		dataArray[0] = dataArray[0] >> 1;
		dataArray[0] = (dataArray[0] << 1);
		dataArray[0] |= 1;
		break;

	case False:
		dataArray[0] = dataArray[0] >> 1;
		dataArray[0] = (dataArray[0] << 1);
		break;
	}

#if DebugModus == true
	ErrorCheck(serverSocket.Send(netAddress, (char*)dataArray, lengthArrayToSend).m_errorCode);
#else
	serverSocket.Send(netAddress, (char*)dataArray, lengthArrayToSend);
#endif
}

void BCServer::RoomRequest(NetAddress& p_receiveAddress, char* p_receiveArray, unsigned short p_rounds, unsigned short p_gameTime)
{
	Print("Received \"search for room\" request from: ");
	for (int i = 0; (i < 20) && (p_receiveArray[i + 2] != -52); i++)
		Print(p_receiveArray[i + 2]);
	Println("");


	//check if allready in a room
	for (p_gameTime = 0; p_gameTime < 3; ++p_gameTime)
	{
		for (p_rounds = 0; p_rounds < 3; ++p_rounds)
		{

			for (unsigned int roomCounter = 0; roomCounter < roomList[p_rounds * 3 + p_gameTime].size(); ++roomCounter)
			{
				if (roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->FindClient(p_receiveAddress))
				{
					if (p_receiveAddress == roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_netaddress)
					{
						CharArrayAddChar(p_receiveArray, 2, roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Member->m_nickname, sizeof(roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Member->m_nickname));
					}
					else
					{
						CharArrayAddChar(p_receiveArray, 2, roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_nickname, sizeof(roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_nickname));
					}
					p_receiveArray[1] = roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_roomID;
					SendData(p_receiveAddress, True, p_receiveArray, 22);
					Print("Player allready in a room (ID: " << (int)p_receiveArray[1] << ")");
					return;
				}
			}
		}
	}

	p_rounds = p_receiveArray[1] >> 5;
	p_gameTime = p_receiveArray[1] << 3;
	p_gameTime = p_gameTime >> 5;

	if (p_rounds == 0 || p_gameTime == 0)
	{
		if (p_rounds == 0)
		{
			if (p_gameTime == 0)
			{
				return;
			}
		}
	}
	else
	{
		--p_rounds;
		--p_gameTime;

		//find room for player
		for (unsigned int roomCounter = 0; roomCounter < roomList[p_rounds * 3 + p_gameTime].size(); ++roomCounter)
		{
			if (!roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_full)
			{
				p_receiveArray[0] = 0;
				p_receiveArray[1] = roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_roomID;

				roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->AddRival(new BCClient(p_receiveAddress, p_receiveArray));

				CharArrayAddChar(p_receiveArray, 2, roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_nickname, 20);
				SendData(roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Member->m_netaddress, True, p_receiveArray, 22);


				p_receiveArray[0] = 1 << 1;
				CharArrayAddChar(p_receiveArray, 1, roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Member->m_nickname, 20);
				SendDataBCM(roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner, True, p_receiveArray, 22);

				Print("Player joined room with ID ");
				Println(roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_roomID);
				return;
			}
		}
	}

	SendData(p_receiveAddress, False, p_receiveArray, 1);
	Println("Room request failed");
	return;
}
void BCServer::CreateRoom(NetAddress& p_receiveAddress, char* p_receiveArray, unsigned short p_rounds, unsigned short p_gameTime)
{
	Print("Received \"create room\" request from: ");
	for (int i = 0; (i < 20) && ((int)p_receiveArray[i + 2] != 0); i++)
		Print(p_receiveArray[i + 2]);
	Println("");

	//check if adress already in a room
	for (p_gameTime = 0; p_gameTime < 3; ++p_gameTime)
	{
		for (p_rounds = 0; p_rounds < 3; ++p_rounds)
		{
			for (unsigned int roomCounter = 0; roomCounter < roomList[p_rounds * 3 + p_gameTime].size(); ++roomCounter)
			{
				if (roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->FindClient(p_receiveAddress))
				{
					p_receiveArray[1] = roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_roomID;
					SendData(p_receiveAddress, True, p_receiveArray, 2);
					Print("Room found with ID ");
					Println((int)p_receiveArray[1]);
					return;
				}
			}
		}
	}

	p_rounds = p_receiveArray[1] >> 5;
	p_gameTime = p_receiveArray[1] << 3;
	p_gameTime = p_gameTime >> 5;

	--p_rounds;
	--p_gameTime;

	p_receiveArray[1] = BCRoom(new BCClient(p_receiveAddress, p_receiveArray), p_rounds, p_gameTime).m_roomID;

	SendData(p_receiveAddress, True, p_receiveArray, 2);
	Print("Room created with ID ");
	Println((int)p_receiveArray[1]);
}
void BCServer::LeaveRoom(NetAddress& p_receiveAddress, char* p_receiveArray, unsigned short p_rounds, unsigned short p_gameTime)
{
	unsigned int a = (int)p_receiveArray[1];

	roomIDList->at(a).RemoveRival(p_receiveAddress, p_receiveArray);
}

void BCServer::HeartBeat(NetAddress& receiveAddress, char* receiveArray)
{
	clientIDList->at(receiveArray[1]).m_ping = messageIDList->at(receiveArray[0] >> 1).m_timeStamp - GetTimeInMilli();
	messageIDList->erase(receiveArray[0] >> 1);
}
