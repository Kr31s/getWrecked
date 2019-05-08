#include "ServerDefines.h"

#include "BCMessage.h"
#include "BCRoom.h"

static std::vector<BCRoom*> roomList[3][3];
static std::map<unsigned int, BCRoom> roomIDList;
static std::vector<BCMessage> messageListe;

static NetSocketUDP serverSocket;
NetAddress receiveAddress;

static char receiveArray[50] = { 0 };

unsigned char rounds = 0;
unsigned char gameTime = 0;
unsigned char identifier = NULL;
unsigned char status = NULL;

long long GetTimeInMilli()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
//BCClient needed to add it to controllMessage
void SendDataBCM(BCClient& receiver, SendType p_status, char* dataArray, unsigned int lengthArrayToSend)
{
	messageListe.push_back(BCMessage(receiver, GetTimeInMilli(), dataArray, lengthArrayToSend));

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
	serverSocket.Send(receiver.m_netaddress, (char*)dataArray, lengthArrayToSend).m_errorCode;
	//Println(serverSocket.Send(receiver, (char*)dataArray, lengthArrayToSend).m_errorCode);
}
//Only address needed, no controllMessage
void SendData(NetAddress& netAddress, SendType p_status, char* dataArray, unsigned int lengthArrayToSend)
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
	serverSocket.Send(netAddress, (char*)dataArray, lengthArrayToSend).m_errorCode;
	//Println(serverSocket.Send(receiver, (char*)dataArray, lengthArrayToSend).m_errorCode);
}

void DecodeMessageServer();
void ClearReceiveArray()
{
	for (int i = 0; (receiveArray[i] != NULL) && i < sizeof(receiveArray); ++i)
	{
		receiveArray[i] = NULL;
	}
}

void GetReplyMessage(NetAddress& p_netAddress)
{
	for (int i = 0; i < messageListe.size(); ++i)
	{
		if (messageListe[i].m_receiver->m_netaddress.GetIpv4Ref() == p_netAddress.GetIpv4Ref()
			&& messageListe[i].m_receiver->m_netaddress.GetPortRef() == p_netAddress.GetPortRef())
		{
			messageListe.erase(messageListe.begin() + i);
		}
	}
}
void CheckMessages()
{
	for (int i = 0; i < messageListe.size(); ++i)
	{
		if (messageListe[i].m_timeStamp < GetTimeInMilli() + messageListe[i].m_receiver->m_ping + 20)
		{
			SendDataBCM(*messageListe[i].m_receiver, None, messageListe[i].m_messageArray, messageListe[i].m_messageArrayLength);
		}
	}
}

void MessageThread()
{
	while (serverRunning)
	{
		CheckMessages();
	}
}
void ServerThread()
{
	while (serverRunning)
	{
		receiveAddress = serverSocket.Receive((char*)receiveArray, 50);
		if (receiveAddress.GetPortRef() != NULL)
		{
			DecodeMessageServer();
			ClearReceiveArray();
		}
	}
}

void ReceiveArrayAddString(unsigned int startPos, char* nameArray, unsigned int arrayLength)
{
	for (unsigned int i = startPos; i < (startPos + arrayLength); ++i)
	{
		receiveArray[i] = nameArray[i - startPos];
	}
}

void RoomRequest()
{
	Print("Received \"search for room\" request from: ");
	for (int i = 0; (i < 20) && (receiveArray[i + 2] != -52); i++)
		Print(receiveArray[i + 2]);
	Println("");
	rounds = receiveArray[1] >> 5;
	gameTime = receiveArray[1] << 3;
	gameTime = gameTime >> 5;

	if (rounds == 0 || gameTime == 0)
	{
		if (rounds == 0)
		{
			if (gameTime == 0)
			{

				return;
			}
		}
	}
	else
	{
		--rounds;
		--gameTime;

		for (unsigned int roomCounter = 0; roomCounter < roomList[rounds][gameTime].size(); ++roomCounter)
		{
			if (!roomList[rounds][gameTime][roomCounter]->m_full)
			{
				roomList[rounds][gameTime][roomCounter]->AddRival(BCClient(receiveAddress, receiveArray));
				ReceiveArrayAddString(2, roomList[rounds][gameTime][roomCounter]->m_Owner.m_nickname, sizeof(roomList[rounds][gameTime][roomCounter]->m_Owner.m_nickname));
				receiveArray[1] = roomList[rounds][gameTime][roomCounter]->m_roomID;
				SendData(roomList[rounds][gameTime][roomCounter]->m_Member.m_netaddress, True, receiveArray, 22);

				receiveArray[0] = 1 << 1;
				for (int i = 0; (i < 20); i++)
				{
					receiveArray[i + 1] = roomList[rounds][gameTime][roomCounter]->m_Member.m_nickname[i];
				}

				SendDataBCM(roomList[rounds][gameTime][roomCounter]->m_Owner, True, receiveArray, 22);
				Print("Player joined room with ID ");
				Println(roomList[rounds][gameTime][roomCounter]->m_roomID);
				return;
			}
		}
	}

	SendData(receiveAddress, False, receiveArray, 1);
	Println("Room request failed");
	return;
}
void CreateRoom()
{
	Print("Received \"create room\" request from: ");
	for (int i = 0; (i < 20) && ((int)receiveArray[i + 2] != 0); i++)
		Print(receiveArray[i + 2]);
	Println("");

	rounds = receiveArray[1] >> 5;
	gameTime = receiveArray[1] << 3;
	gameTime = gameTime >> 5;

	//add new room
	for (int i = 0; i <= totalRoomID; ++i)
	{
		if (roomIDList.find(i) == roomIDList.end())
		{
			roomIDList[i] = BCRoom(BCClient(receiveAddress, receiveArray), rounds, gameTime);
			roomList[rounds - 1][gameTime - 1].push_back(&roomIDList[(i)]);
			receiveArray[1] = i;

			if (i == totalRoomID)
			{
				++totalRoomID;
			}
			break;
		}
	}

	SendData(receiveAddress, True, receiveArray, 2);
	Print("Room created with ID ");
	Println((int)receiveArray[1]);
}
void LeaveRoom()
{
	unsigned int a = (int)receiveArray[1];
	unsigned int roundState = roomIDList[a].m_roundState - 1;
	unsigned int timeState = roomIDList[a].m_timeState - 1;

	if (roomIDList[a].m_Owner.m_netaddress.GetIpv4Ref() == receiveAddress.GetIpv4Ref() && roomIDList[a].m_Owner.m_netaddress.GetPortRef() == receiveAddress.GetPortRef())
	{

		if (!roomIDList[a].m_full)
		{
			for (int i = 0; i < roomList[roundState][timeState].size(); ++i)
			{
				if (roomList[roundState][timeState][i]->m_roomID == roomIDList[a].m_roomID)
				{
					SendData(roomIDList[a].m_Owner.m_netaddress, True, receiveArray, 1);

					roomList[roundState][timeState].erase(roomList[roundState][timeState].begin() + i);
					roomIDList.erase(a);

					Println("One player left the room with the ID " << a);
					Println("The room with the ID " << a << " was deleted");
					return;
				}
			}
		}
		else
		{
			SendData(roomIDList[a].m_Owner.m_netaddress, True, receiveArray, 1);
			receiveArray[0] = 4 << 1;
			SendDataBCM(roomIDList[a].m_Member, True, receiveArray, 1);

			roomIDList[a].m_Owner = roomIDList[a].m_Member;
			roomIDList[a].m_Member = BCClient();
			roomIDList[a].m_full = false;
			Println("One player left the room with the ID " << a);
		}
		SendData(roomIDList[a].m_Owner.m_netaddress, False, receiveArray, 1);
	}
	else if (roomIDList[a].m_Member.m_netaddress.GetIpv4Ref() == receiveAddress.GetIpv4Ref() && roomIDList[a].m_Member.m_netaddress.GetPortRef() == receiveAddress.GetPortRef())
	{
		if (!roomIDList[a].m_full)
		{
			for (int i = 0; i < roomList[roundState][timeState].size(); ++i)
			{
				if (roomList[roundState][timeState][i]->m_roomID == roomIDList[a].m_roomID)
				{
					SendData(roomIDList[a].m_Member.m_netaddress, True, receiveArray, 1);

					roomList[roundState][timeState].erase(roomList[roundState][timeState].begin() + i);
					roomIDList.erase(a);
					Println("One player left the room with the ID " << a);
					Println("The room with the ID " << a << " was deleted");
					return;
				}
			}
		}
		else
		{
			SendData(roomIDList[a].m_Member.m_netaddress, True, receiveArray, 1);
			receiveArray[0] = 4 << 1;
			SendDataBCM(roomIDList[a].m_Owner, True, receiveArray, 1);

			roomIDList[a].m_Member = BCClient();
			roomIDList[a].m_full = false;
			Print("One player left the room with the ID " << a);
			return;
		}

		SendData(roomIDList[a].m_Member.m_netaddress, False, receiveArray, 1);
	}


}
void NonServerMessage()
{

}

void DecodeMessageServer()
{
	identifier = receiveArray[0] >> 1;
	status = receiveArray[0] << 7;
	status = status >> 7;

	if (status == 0)
	{
		switch (identifier)
		{
		case 0:
			RoomRequest();
			break;
		case 2:
			CreateRoom();
			break;
		case 3:
			LeaveRoom();
			break;
		default:
			NonServerMessage();
			break;
		}
	}
	else
	{
		GetReplyMessage(receiveAddress);
	}
}

void main()
{
	Print("InitializeSocketLayer: ");
	Println(((BWNet::InitializeSocketLayer().m_errorCode == 0) ? "Succes" : "Failed"));
	Print("OpenSocket 4405: ");
	Println(((serverSocket.OpenSocket(4405).m_errorCode == 0) ? "Succes" : "Failed"));
	Print("Is Socket 4405 open ? ");
	Println(((serverSocket.IsOpen() == 1) ? "true" : "false"));
	Print("Enable Non-Blocking: ");
	Println(((serverSocket.EnableNonBlocking().m_errorCode == 0) ? "Succes" : "Failed"));
	Println("");

	std::thread t1(ServerThread);
	std::thread t2(MessageThread);


	int r;
	std::cin >> r;
}
