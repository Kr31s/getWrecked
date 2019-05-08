#pragma once
#include "ServerDefines.h"


class BCServer
{
public:
	bool*								serverRunning	= new bool(true);

	NetSocketUDP						serverSocket;

	unsigned int*						totalRoomID		= new unsigned int(0);

	std::vector<BCRoom&>*				roomList		= new std::vector<BCRoom&>[3 * 3]; //roomList[i * sizeof(j) + j] insane
	std::map<unsigned int, BCRoom&>*	roomIDList		= new std::map<unsigned int, BCRoom&>();
	std::vector<BCMessage&>*			messageListe	= new std::vector<BCMessage&>();

	BCServer(unsigned short port, bool enableNonBlocking);
	~BCServer();

	//BCClient needed to add it to controllMessage
	void SendDataBCM(BCClient* receiver, SendType p_status, char* dataArray, unsigned int lengthArrayToSend);
	//Only address needed, no controllMessage
	void SendData(NetAddress& netAddress, SendType p_status, char* dataArray, unsigned int lengthArrayToSend);



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
					roomList[rounds][gameTime][roomCounter]->AddRival(new BCClient(receiveAddress, receiveArray));
					ReceiveArrayAddString(2, roomList[rounds][gameTime][roomCounter]->m_Owner->m_nickname, sizeof(roomList[rounds][gameTime][roomCounter]->m_Owner->m_nickname));
					receiveArray[1] = roomList[rounds][gameTime][roomCounter]->m_roomID;
					SendData(roomList[rounds][gameTime][roomCounter]->m_Member->m_netaddress, True, receiveArray, 22);

					receiveArray[0] = 1 << 1;
					for (int i = 0; (i < 20); i++)
					{
						receiveArray[i + 1] = roomList[rounds][gameTime][roomCounter]->m_Member->m_nickname[i];
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
		for (int i = 0; i <= *totalRoomID; ++i)
		{
			if (roomIDList.find(i) == roomIDList.end())
			{
				roomIDList->at(i) = *new BCRoom(new BCClient(receiveAddress, receiveArray), rounds, gameTime);
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
};

