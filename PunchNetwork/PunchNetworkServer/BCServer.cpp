#include "BCServer.h"
#include "BCRoom.h"
#include "BCClient.h"
#include "BCMessage.h"
#include "BCMessage.h"
#include <bitset>

BCServer* BCServer::sTheServer = nullptr;

BCServer::BCServer(unsigned short p_port, bool p_enableNonBlocking)
{
	Print("InitializeSocketLayer: ");
	Println(((BWNet::InitializeSocketLayer().m_errorCode == 0) ? "Succes" : "Failed"));
	Print("OpenSocket 4405: ");
	Println(((m_serverSocket.OpenSocket(p_port).m_errorCode == 0) ? "Succes" : "Failed"));
	Print("Is Socket 4405 open ? ");
	Println(((m_serverSocket.IsOpen() == 1) ? "true" : "false"));

	if (p_enableNonBlocking)
	{
		Print("Enable Non-Blocking: ");
		Println(((m_serverSocket.EnableNonBlocking().m_errorCode == 0) ? "Succes" : "Failed"));
	}
	Println("");
}
BCServer::~BCServer()
{
	delete(m_roomIDList);
	delete(m_clientIDList);
	delete(m_messageIDList);
	delete[](BCServer::sTheServer->m_roomList);
	delete(m_serverRunning);
}

void BCServer::deleteClient(BCClient * p_client, char* p_receiveArray)
{
	for (int i = 0; i < BCMessage::sTotalMessageID; ++i)
	{
		if (BCServer::sTheServer->m_messageIDList->find(i) == BCServer::sTheServer->m_messageIDList->end())
		{
			continue;
		}
		if (BCServer::sTheServer->m_messageIDList->at(i).m_clientID == p_client->m_clientID)
		{
			BCServer::sTheServer->m_messageIDList->at(i).m_finished = true;
		}
	}
	BCServer::sTheServer->m_clientIDList->erase(p_client->m_clientID);
}
void BCServer::deleteRoom(BCRoom * p_room, char* p_receiveArray)
{
	for (int i = 0; i < BCServer::sTheServer->m_roomList[(p_room->m_roundState) * 3 + (p_room->m_timeState)].size(); ++i)
	{
		if (BCServer::sTheServer->m_roomList[(p_room->m_roundState) * 3 + (p_room->m_timeState)].at(i)->m_roomID == p_room->m_roomID)
		{
			if (p_room->m_Owner != nullptr)
				BCServer::sTheServer->SendData(p_room->m_Owner->m_netaddress, SendType::True, p_receiveArray);

			BCServer::sTheServer->m_roomList[(p_room->m_roundState) * 3 + (p_room->m_timeState)].erase(BCServer::sTheServer->m_roomList[(p_room->m_roundState) * 3 + (p_room->m_timeState)].begin() + i);

			Println("One player left the room with the ID " << p_room->m_roomID);
			Println("The room with the ID " << p_room->m_roomID << " was deleted");
			BCServer::sTheServer->m_roomIDList->erase(p_room->m_roomID);
			Println("Room complete deleted");
			break;
		}
	}
}

void BCServer::SendDataBCM(unsigned int p_clientID, SendType p_status, char* p_dataArray)
{
	p_dataArray[45] = BCMessage(p_clientID, GetTimeInMilli(), p_dataArray).m_messageID;

	switch (p_status)
	{
	case SendType::None:
		break;

	case SendType::True:
		p_dataArray[1] = 1;
		break;

	case SendType::False:
		p_dataArray[1] = 0;
		break;
	}

#if DebugModus == true
	ErrorCheck(serverSocket.Send(BCServer::theServer->clientIDList->at(p_clientID).m_netaddress, (char*)dataArray, 46).m_errorCode);
#else
	m_serverSocket.Send(BCServer::sTheServer->m_clientIDList->at(p_clientID).m_netaddress, (char*)p_dataArray, 46);
#endif
}
void BCServer::SendData(NetAddress & p_netAddress, SendType p_status, char* p_dataArray)
{
	switch (p_status)
	{
	case SendType::None:
		break;

	case SendType::True:
		p_dataArray[1] = 1;
		break;

	case SendType::False:
		p_dataArray[1] = 0;
		break;
	}

#if DebugModus == true
	ErrorCheck(serverSocket.Send(netAddress, (char*)dataArray, 46).m_errorCode);
#else
	m_serverSocket.Send(p_netAddress, (char*)p_dataArray, 46);
#endif
}


void BCServer::RoomRequest(NetAddress & p_receiveAddress, char* p_receiveArray, unsigned char& p_rounds, unsigned char& p_gameTime)
{
	Print("Received \"search for room\" request from: ");
	for (int i = 0; (i < 20) && (p_receiveArray[i + 4] != NULL); i++)
		Print(p_receiveArray[i + 4]);
	Println("");


	//check if allready in a room
	for (p_gameTime = 0; p_gameTime < 3; ++p_gameTime)
	{
		for (p_rounds = 0; p_rounds < 3; ++p_rounds)
		{
			for (unsigned int roomCounter = 0; roomCounter < BCServer::sTheServer->BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].size(); ++roomCounter)
			{
				if (BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->FindClient(p_receiveAddress))
				{
					if (p_receiveAddress == BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_netaddress)
					{
						CharArrayAddChar(p_receiveArray, 2, BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Member->m_nickname,0, sizeof(BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Member->m_nickname));
					}
					else
					{
						CharArrayAddChar(p_receiveArray, 2, BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_nickname,0, sizeof(BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_nickname));
					}
					p_receiveArray[1] = BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_roomID;
					p_receiveArray[2] = BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->GetClient(p_receiveAddress)->m_clientID;
					SendData(p_receiveAddress, SendType::True, p_receiveArray);
					Print("Player allready in a room (ID: " << (int)p_receiveArray[1] << ")");
					return;
				}
			}
		}
	}

	p_rounds = p_receiveArray[2];
	p_gameTime = p_receiveArray[3];

	if (p_rounds == 0 || p_gameTime == 0)
	{
		if (p_rounds == 0)
		{
			if (p_gameTime == 0)
			{
				//Any state
				return;
			}
		}
	}
	else
	{
		--p_rounds;
		--p_gameTime;

		//find room for player
		for (unsigned int roomCounter = 0; roomCounter < BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].size(); ++roomCounter)
		{
			if (!BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_full)
			{
				//Found Room
				//write message to tell the requested the status of his message
				p_receiveArray[0] = 0;
				sMutexClientIDList.lock();
				p_receiveArray[3] = BCClient(p_receiveAddress, p_receiveArray).m_clientID;
				sMutexClientIDList.unlock();
				p_receiveArray[2] = BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_roomID;

				//add the person who reqested to the room
				BCServer::sTheServer->m_roomIDList->at(p_receiveArray[2]).AddRival(&BCServer::sTheServer->m_clientIDList->at(p_receiveArray[3]));

				CharArrayAddChar(p_receiveArray, 4, BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_nickname,0, 20);
				SendData(BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Member->m_netaddress, SendType::True, p_receiveArray);

				//write message to inform the owner of the room
				p_receiveArray[0] = 1;
				CharArrayAddChar(p_receiveArray, 2, BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Member->m_nickname,0, 20);
				SendDataBCM(BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_clientID, SendType::True, p_receiveArray);

				Print("Player joined room with ID ");
				Println(BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_roomID);
				return;
			}
		}
	}

	SendData(p_receiveAddress, SendType::False, p_receiveArray);
	Println("Room request failed");
	return;
}
void BCServer::CreateRoom(NetAddress & p_receiveAddress, char* p_receiveArray, unsigned char& p_rounds, unsigned char& p_gameTime)
{
	Print("Received \"create room\" request from: ");
	for (int i = 0; (i < 20) && (p_receiveArray[i + 4] != NULL); i++)
		Print(p_receiveArray[i + 4]);
	Println("");

	//check if adress already in a room
	for (p_gameTime = 0; p_gameTime < 3; ++p_gameTime)
	{
		for (p_rounds = 0; p_rounds < 3; ++p_rounds)
		{
			for (unsigned int roomCounter = 0; roomCounter < BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].size(); ++roomCounter)
			{
				if (BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->FindClient(p_receiveAddress))
				{
					p_receiveArray[1] = 0;
					p_receiveArray[2] = BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_roomID;
					p_receiveArray[3] = BCServer::sTheServer->m_roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->GetClient(p_receiveAddress)->m_clientID;
					SendData(p_receiveAddress, SendType::True, p_receiveArray);
					Print("Room found with ID ");
					Println((int)p_receiveArray[2]);
					return;
				}
			}
		}
	}

	p_rounds = p_receiveArray[2];
	p_gameTime = p_receiveArray[3];

	--p_rounds;
	--p_gameTime;
	p_receiveArray[1] = 0;
	sMutexClientIDList.lock();
	p_receiveArray[3] = BCClient(p_receiveAddress, p_receiveArray).m_clientID;
	sMutexClientIDList.unlock();
	p_receiveArray[2] = BCRoom(&BCServer::sTheServer->m_clientIDList->at(p_receiveArray[3]), p_rounds, p_gameTime).m_roomID;
	SendData(p_receiveAddress, SendType::True, p_receiveArray);
	Print("Room created with ID ");
	Println((int)p_receiveArray[1]);
}
void BCServer::LeaveRoom(NetAddress & p_receiveAddress, char* p_receiveArray)
{
	SendData(p_receiveAddress, SendType::True, p_receiveArray);
	m_roomIDList->at((int)p_receiveArray[2]).RemoveRival(p_receiveAddress, p_receiveArray);
}
void BCServer::ElementChange(NetAddress & p_receiveAddress, char* p_receiveArray)
{
	if (BCServer::sTheServer->m_roomIDList->at(p_receiveArray[2]).FindClient(p_receiveAddress))
	{
		//net address is in room
		if (BCServer::sTheServer->m_roomIDList->at(p_receiveArray[2]).IsOwner(p_receiveAddress))
		{
			Println("Value change Owner");

			if (BCServer::sTheServer->m_roomIDList->at(p_receiveArray[2]).m_Member == nullptr)
				return;

			p_receiveArray[40] = p_receiveArray[2];
			p_receiveArray[0] = 7;
			p_receiveArray[1] = p_receiveArray[2];
			p_receiveArray[2] = p_receiveArray[3];
			p_receiveArray[3] = p_receiveArray[4];
			SendDataBCM(BCServer::sTheServer->m_roomIDList->at(p_receiveArray[40]).m_Member->m_clientID, SendType::True, p_receiveArray);
		}
		else
		{
			Println("Value change Member");


			if (BCServer::sTheServer->m_roomIDList->at(p_receiveArray[1]).m_Owner == nullptr)
				return;

			p_receiveArray[40] = p_receiveArray[2];
			p_receiveArray[0] = 7;
			p_receiveArray[1] = p_receiveArray[2];
			p_receiveArray[2] = p_receiveArray[3];
			p_receiveArray[3] = p_receiveArray[4];
			SendDataBCM(BCServer::sTheServer->m_roomIDList->at(p_receiveArray[40]).m_Owner->m_clientID, SendType::True, p_receiveArray);
		}
	}
}
void BCServer::PauseGame(NetAddress & p_receiveAddress, char* p_receiveArray)
{
	p_receiveArray[0] = 9;

	if (m_roomIDList->at(p_receiveArray[1]).m_gamePaused)
	{
		SendDataBCM(m_roomIDList->at(p_receiveArray[1]).GetRival(p_receiveAddress)->m_clientID, SendType::False, p_receiveArray);
		SendData(p_receiveAddress, SendType::False, p_receiveArray);
	}
	else
	{
		SendDataBCM(m_roomIDList->at(p_receiveArray[1]).GetRival(p_receiveAddress)->m_clientID, SendType::True, p_receiveArray);
		SendData(p_receiveAddress, SendType::True, p_receiveArray);
	}

	m_roomIDList->at(p_receiveArray[1]).m_gamePaused = !m_roomIDList->at(p_receiveArray[1]).m_gamePaused;
}
void BCServer::GameMessage(NetAddress & p_receiveAddress, char* p_receiveArray, unsigned int& p_intValue)
{
	for (int i = 0; i < 14; ++i)
	{
		p_intValue = (int)(p_receiveArray[2 + 4 * i] << 8);
		p_intValue |= (int)(p_receiveArray[3 + 4 * i]);

		if (m_roomIDList->at(p_receiveArray[1]).GetClient(p_receiveAddress)->m_lastClientFrame + 1 == p_intValue)
		{
			p_receiveArray[0] = 11;
			SendDataBCM(m_roomIDList->at(p_receiveArray[1]).GetRival(p_receiveAddress)->m_clientID, SendType::False, p_receiveArray);

			m_roomIDList->at(p_receiveArray[1]).GetClient(p_receiveAddress)->m_lastClientFrame = p_intValue;
			return;
		}
	}
	Println("Lost GameInput");
}