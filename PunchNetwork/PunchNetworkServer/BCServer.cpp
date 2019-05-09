#include "BCServer.h"
#include "BCRoom.h"
#include "BCClient.h"
#include "BCMessage.h"


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

void BCServer::SendDataBCM(BCClient* receiver, SendType p_status, char* dataArray, unsigned int lengthArrayToSend)
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

		for (unsigned int roomCounter = 0; roomCounter < roomList[p_rounds * 3 + p_gameTime].size(); ++roomCounter) //roomList[rounds][gameTime].size()
		{
			if (!roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_full)
			{
				roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->AddRival(new BCClient(p_receiveAddress, p_receiveArray));
				ReceiveArrayAddString(p_receiveArray, 2, roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_nickname, sizeof(roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Owner->m_nickname));
				p_receiveArray[1] = roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_roomID;
				SendData(roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Member->m_netaddress, True, p_receiveArray, 22);

				p_receiveArray[0] = 1 << 1;
				for (int i = 0; (i < 20); i++)
				{
					p_receiveArray[i + 1] = roomList[p_rounds * 3 + p_gameTime].at(roomCounter)->m_Member->m_nickname[i];
				}

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
	unsigned int roundState = roomIDList->at(a).m_roundState - 1;
	unsigned int timeState = roomIDList->at(a).m_timeState - 1;

	if (roomIDList->at(a).m_Owner->m_netaddress.GetIpv4Ref() == p_receiveAddress.GetIpv4Ref() && roomIDList->at(a).m_Owner->m_netaddress.GetPortRef() == p_receiveAddress.GetPortRef())
	{

		if (!roomIDList->at(a).m_full)
		{
			for (int i = 0; i < roomList[roundState * 3 + timeState].size(); ++i)
			{
				if (roomList[roundState * 3 + timeState].at(i)->m_roomID == roomIDList->at(a).m_roomID)
				{
					SendData(roomIDList->at(a).m_Owner->m_netaddress, True, p_receiveArray, 1);

					roomList[roundState * 3 + timeState].erase(roomList[roundState * 3 + timeState].begin() + i);
					roomIDList->erase(a);

					Println("One player left the room with the ID " << a);
					Println("The room with the ID " << a << " was deleted");
					return;
				}
			}
		}
		else
		{
			SendData(roomIDList->at(a).m_Owner->m_netaddress, True, p_receiveArray, 1);
			p_receiveArray[0] = 4 << 1;
			SendDataBCM(roomIDList->at(a).m_Member, True, p_receiveArray, 1);

			roomIDList->at(a).m_Owner = roomIDList->at(a).m_Member;
			delete(roomIDList->at(a).m_Member);
			roomIDList->at(a).m_full = false;
			Println("One player left the room with the ID " << a);
		}
		SendData(roomIDList->at(a).m_Owner->m_netaddress, False, p_receiveArray, 1);
	}
	else if (roomIDList->at(a).m_Member->m_netaddress.GetIpv4Ref() == p_receiveAddress.GetIpv4Ref() && roomIDList->at(a).m_Member->m_netaddress.GetPortRef() == p_receiveAddress.GetPortRef())
	{
		if (!roomIDList->at(a).m_full)
		{
			for (int i = 0; i < roomList[roundState * 3 + timeState].size(); ++i)
			{
				if (roomList[roundState * 3 + timeState].at(i)->m_roomID == roomIDList->at(a).m_roomID)
				{
					SendData(roomIDList->at(a).m_Member->m_netaddress, True, p_receiveArray, 1);

					roomList[roundState * 3 + timeState].erase(roomList[roundState * 3 + timeState].begin() + i);
					roomIDList->erase(a);
					Println("One player left the room with the ID " << a);
					Println("The room with the ID " << a << " was deleted");
					return;
				}
			}
		}
		else
		{
			SendData(roomIDList->at(a).m_Member->m_netaddress, True, p_receiveArray, 1);
			p_receiveArray[0] = 4 << 1;
			SendDataBCM(roomIDList->at(a).m_Owner, True, p_receiveArray, 1);

			delete(roomIDList->at(a).m_Member);
			roomIDList->at(a).m_full = false;
			Print("One player left the room with the ID " << a);
			return;
		}
		SendData(roomIDList->at(a).m_Member->m_netaddress, False, p_receiveArray, 1);
	}
}

void BCServer::HeartBeat(NetAddress& receiveAddress, char* receiveArray)
{
	clientIDList->at(receiveArray[1]).m_ping = messageIDList->at(receiveArray[0] >> 1).m_timeStamp - GetTimeInMilli();
	messageIDList->erase(receiveArray[0] >> 1);
}
