#include "Inc_BWNetworking.h"
#include "Inc_SmartMacros.h"
#include "Inc_BWMath.h"

#include <stdexcept>
#include <vector>
#include <thread>
#include <chrono>
#include <bitset>
#include <algorithm>


#define crypt(x) x^89^3<25^79<47^1>55<33

static unsigned int totalRoomID = 0;
static NetSocketUDP serverSocket;
static char receiveArray[50] = { 0 };

struct Client
{
	NetSocketTCP m_clientTCP;
	NetAddress m_netaddress;
	char m_nickname[20];

	Client() {};
	Client(NetAddress p_netaddress, char* p_nickname)
	{
		m_netaddress = p_netaddress;
		for (int i = 2; (p_nickname[i] != NULL) && (i < 22); ++i)
		{
			m_nickname[i - 2] = p_nickname[i];
		}
	}
};
struct PunchRoom
{
	bool m_full = false;
	unsigned short m_roomID;

	unsigned short m_roundState;
	unsigned short m_timeState;

	Client m_Owner;
	Client m_Member;


	PunchRoom(Client p_client, unsigned short p_roundState, unsigned short p_timeState)
	{
		m_Owner = p_client;
		m_roundState = p_roundState;;
		m_timeState = p_timeState;;
		m_roomID = totalRoomID;
		++totalRoomID;
	}
	void AddRival(Client p_client)
	{
		m_Member = p_client;
		m_full = true;
	}
};
struct PunchMessage
{
	NetAddress m_netAddress;
	std::chrono::time_point<std::chrono::system_clock> m_timeStamp;

	char m_messageArray[50];
	unsigned int m_messageArrayLength;

	PunchMessage(NetAddress& p_netAddress, std::chrono::time_point<std::chrono::system_clock> p_timeStamp, char* p_messageArray, unsigned int p_messageArrayLength)
	{
		m_netAddress = p_netAddress;
		m_timeStamp = p_timeStamp;

		for (int i = 0; i < m_messageArrayLength; ++i)
		{
			m_messageArray[i] = p_messageArray[i];
		}

	}
};

static std::vector<PunchRoom*> roomList[3][3];
static std::vector<PunchRoom> roomIDList;
static std::vector<PunchMessage> messageListe;

NetAddress receiveAddress;
unsigned char rounds = 0;
unsigned char gameTime = 0;
unsigned char identifier = NULL;
unsigned char status = NULL;

void DecodeMessageServer();

void ServerThread()
{
	while (true)
	{
		receiveAddress = serverSocket.Receive((char*)receiveArray, 50);
		if (receiveAddress.GetPortRef() != NULL)
		{
			DecodeMessageServer();
		}
	}
}

void SendData(NetAddress receiver, bool p_status, char* dataArray, unsigned int lengthArrayToSend)
{
	dataArray[0] = dataArray[0] >> 1;
	dataArray[0] = (dataArray[0] << 1);
	dataArray[0] |= static_cast<char>(p_status);

	Println(serverSocket.Send(receiver, (char*)dataArray, lengthArrayToSend).m_errorCode);
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
	Print("Received \"room room\" request from: ");
	for (int i = 0; (i < 20) && (receiveArray[i + 2] != -52); i++)
		Print(receiveArray[i + 2]);

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
				roomList[rounds][gameTime][roomCounter]->AddRival(Client(receiveAddress, receiveArray));
				ReceiveArrayAddString(2, roomList[rounds][gameTime][roomCounter]->m_Owner.m_nickname, sizeof(roomList[rounds][gameTime][roomCounter]->m_Owner.m_nickname));
				receiveArray[1] = crypt((roomList[rounds][gameTime][roomCounter]->m_roomID));
				SendData(receiveAddress, true, receiveArray, 22);//first id next room adjustments and 20 for opponent name
				receiveArray[0] = 1 << 1;

				for (int i = 0; (i < 20) && (roomList[rounds][gameTime][roomCounter]->m_Member.m_nickname[i] != -52); i++)
				{
					receiveArray[i + 1] = roomList[rounds][gameTime][roomCounter]->m_Member.m_nickname[i];
				}

				SendData(roomList[rounds][gameTime][roomCounter]->m_Owner.m_netaddress, true, receiveArray, 22);
				Print("Player joined room with ID ");
				Println((int)receiveArray[1]);
				return;
			}
		}
	}

	SendData(receiveAddress, false, receiveArray, 1);//first id next room adjustments and 20 for opponent name
	Println("Room request failed");
	return;
}
void CreateRoom()
{
	Print("Received \"create room\" request from: ");
	for (int i = 0; (i < 20) && (receiveArray[i + 2] != -52); i++)
		Print(receiveArray[i + 2]);
	Println("");

	rounds = receiveArray[1] >> 5;
	gameTime = receiveArray[1] << 3;
	gameTime = gameTime >> 5;

	roomIDList.push_back(PunchRoom(Client(receiveAddress, receiveArray), rounds, gameTime));
	roomList[rounds - 1][gameTime - 1].push_back(&roomIDList[totalRoomID - 1]);
	receiveArray[1] = crypt((totalRoomID - 1));

	messageListe.push_back(PunchMessage(receiveAddress, std::chrono::system_clock::now(), receiveArray, 2));
	SendData(receiveAddress, true, receiveArray, 2);
	Print("Room created with ID ");
	Println((int)receiveArray[1]);
}
void LeaveRoom()
{
	unsigned int a = (crypt((int)receiveArray[1]));
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
					SendData(roomIDList[a].m_Owner.m_netaddress, true, receiveArray, 1);

					roomList[roundState][timeState].erase(roomList[roundState][timeState].begin() + i);
					roomIDList.erase(roomIDList.begin() + a);
					return;
				}
			}
		}
		else
		{
			SendData(roomIDList[a].m_Owner.m_netaddress, true, receiveArray, 1);
			receiveArray[0] = 4 << 1;
			SendData(roomIDList[a].m_Member.m_netaddress, true, receiveArray, 1);

			roomIDList[a].m_Owner = roomIDList[a].m_Member;
			roomIDList[a].m_Member = Client();
			roomIDList[a].m_full = false;
		}
		SendData(roomIDList[a].m_Owner.m_netaddress, false, receiveArray, 1);
	}
	else if (roomIDList[a].m_Member.m_netaddress.GetIpv4Ref() == receiveAddress.GetIpv4Ref() && roomIDList[a].m_Member.m_netaddress.GetPortRef() == receiveAddress.GetPortRef())
	{
		if (!roomIDList[a].m_full)
		{
			for (int i = 0; i < roomList[roundState][timeState].size(); ++i)
			{
				if (roomList[roundState][timeState][i]->m_roomID == roomIDList[a].m_roomID)
				{
					SendData(roomIDList[a].m_Member.m_netaddress, true, receiveArray, 1);

					roomList[roundState][timeState].erase(roomList[roundState][timeState].begin() + i);
					roomIDList.erase(roomIDList.begin() + a);
					return;
				}
			}
		}
		else
		{

			SendData(roomIDList[a].m_Member.m_netaddress, true, receiveArray, 1);
			receiveArray[0] = 4 << 1;
			SendData(roomIDList[a].m_Owner.m_netaddress, true, receiveArray, 1);

			roomIDList[a].m_Member = Client();
			roomIDList[a].m_full = false;
			return;
		}

		SendData(roomIDList[a].m_Member.m_netaddress, false, receiveArray, 1);
	}


}
void NonServerMessage()
{

}

void SearchForReplyMessage(NetAddress p_netAddress)
{
	for (int i = 0; i < messageListe.size(); ++i)
	{
		if (messageListe[i].m_netAddress.GetIpv4Ref() == p_netAddress.GetIpv4Ref())
		{
			messageListe.erase(messageListe.begin() + i);
		}
	}
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
		SearchForReplyMessage(receiveAddress);
	}
}

void ClearReceiveArray()
{
	for (int i = 0; (receiveArray[i] != NULL) && i < sizeof(receiveArray); ++i)
	{
		receiveArray[i] = NULL;
	}
}

void main()
{
	Print("InitializeSocketLayer: ");
	Println(BWNet::InitializeSocketLayer().m_errorCode);
	Print("OpenSocket 4405: ");
	Println(serverSocket.OpenSocket(4405).m_errorCode);
	Print("Is Socket 4405 open ? ");
	Println(((serverSocket.IsOpen() == 1) ? "true" : "false"));
	Print("Enable Non-Blocking: ");
	Println(serverSocket.EnableNonBlocking().m_errorCode);

	std::thread t1(ServerThread);


	int r;
	std::cin >> r;
}
