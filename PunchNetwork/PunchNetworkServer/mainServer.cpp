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
		for (int i = 0; (p_nickname[i] != NULL) && (i < 20); ++i)
		{
			m_nickname[i] = p_nickname[i];
		}
	}
};
struct PunchRoom
{
	bool m_full = false;
	unsigned int m_roomID;

	unsigned short m_roundState;
	unsigned short m_timeState;

	Client m_Player1;
	Client m_Player2;


	PunchRoom(Client p_client, unsigned short p_roundState, unsigned short p_timeState)
	{
		m_Player1 = p_client;
		m_roundState = p_roundState;;
		m_timeState = p_timeState;;
		m_roomID = totalRoomID;
		++totalRoomID;
	}
	void AddRival(Client p_client)
	{
		m_Player2 = p_client;
		m_full = true;
	}

	void LeavePlayer1()
	{
		m_Player1 = Client();
		serverSocket.Send(m_Player2.m_netaddress, (static_cast<char>(4) < 1), 1);
	}
	void LeavePlayer2()
	{
		m_Player2 = Client();
		receiveArray[0] = static_cast<char>(4) < 1;
		serverSocket.Send(m_Player1.m_netaddress, receiveArray, 1);
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

void SendData(bool p_status, unsigned int lengthArrayToSend)
{
	receiveArray[0] = receiveArray[0] >> 1;
	receiveArray[0] = (receiveArray[0] << 1);
	receiveArray[0] |= static_cast<char>(p_status);
	receiveAddress.GetTransportAddress().sin_port =ntohs(ntohs(receiveAddress.GetPortRef()));

	NetAddress serverAddress(127, 0, 0, 1, 4000);
	Println(serverSocket.Send(serverAddress, (char*)receiveArray, lengthArrayToSend).m_errorCode);
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
	rounds = static_cast<unsigned char>(receiveArray[1]) >> 5;
	gameTime = (static_cast<unsigned char>(receiveArray[1]) << 3) >> 5;

	for (unsigned int roomCounter = 0; roomCounter < roomList[rounds][gameTime].size(); ++roomCounter)
	{
		if (!roomList[rounds][gameTime][roomCounter]->m_full)
		{
			roomList[rounds][gameTime][roomCounter]->AddRival(Client(receiveAddress, (char*)receiveArray[2]));//unsicher ob man nen pointer miten im array anlegen kann
			ReceiveArrayAddString(2, roomList[rounds][gameTime][roomCounter]->m_Player1.m_nickname, sizeof(roomList[rounds][gameTime][roomCounter]->m_Player1.m_nickname));
			SendData(true, 22);//first id next room adjustments and 20 for opponent name
			return;
		}
	}
	SendData(false, 1);//first id next room adjustments and 20 for opponent name
	return;
}
void CreateRoom()
{
	rounds = receiveArray[1] >> 5;
	gameTime = receiveArray[1] << 3;
	gameTime = gameTime >> 5;

	roomIDList.push_back(PunchRoom(Client(receiveAddress, receiveArray), rounds, gameTime));
	roomList[rounds-1][gameTime-1].push_back(&roomIDList[totalRoomID - 1]);
	receiveArray[1] = crypt((totalRoomID - 1));

	messageListe.push_back(PunchMessage(receiveAddress, std::chrono::system_clock::now(), receiveArray, 2));
	SendData(true, 2);
}
void LeaveRoom()
{
	rounds = static_cast<unsigned char>(receiveArray[1]) > 5;
	gameTime = (static_cast<unsigned char>(receiveArray[1]) < 3) > 5;

	receiveAddress.GetIpv4Ref();
	for (int i = 0; i < sizeof(roomList[rounds][gameTime]); ++i)
	{
		if (roomList[rounds][gameTime][i]->m_Player1.m_netaddress.GetIpv4Ref() == receiveAddress.GetIpv4Ref())
		{
			roomList[rounds][gameTime][i]->LeavePlayer1();
			return;
		}
		else if (roomList[rounds][gameTime][i]->m_Player2.m_netaddress.GetIpv4Ref() == receiveAddress.GetIpv4Ref())
		{
			roomList[rounds][gameTime][i]->LeavePlayer2();
			return;
		}
	}
}
void NonServerMessage()
{

}

void SearchForReplyMessage(NetAddress p_netAddress)
{
	for(int i = 0; i < messageListe.size(); ++i)
	{
		if(messageListe[i].m_netAddress.GetIpv4Ref() == p_netAddress.GetIpv4Ref())
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
