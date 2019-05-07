#include "Inc_BWNetworking.h"
#include "Inc_SmartMacros.h"
#include "Inc_BWMath.h"

#include <thread>
#include <string>

void ReceiveThread();
void SendRequestClient(int messageType, NetSocketUDP socketUDP);
void ReceiveMessageClient();
void SendReceiveMessageClient();
void ClearReceiveArray();


NetSocketUDP socketUDP;
NetAddress serverAddress(127, 0, 0, 1, 4405);
NetAddress receiveAddress;

unsigned char opponentName[20];

unsigned char sendArray[50];
char receiveArray[50];

int myRoomID = -1;
unsigned char identifier = NULL;
unsigned char status = NULL;
int input;
char* charInput = new char[20];

void main()
{

	BWNet::InitializeSocketLayer();

	Print("ErrorCode OpenSocket ");
	Println(socketUDP.OpenSocket(0).m_errorCode);
	Println("");
	socketUDP.EnableNonBlocking();

	std::thread t1(ReceiveThread);

	Println("0: Room request");
	Println("2: Create room");
	Println("3: Leave room");
	Println("");
	Print("Your Action: ");
	do
	{
		std::cin >> input;
		Println("");
		SendRequestClient(input, socketUDP);
	} while (true);
}

void ReceiveThread()
{
	while (true)
	{
		receiveAddress = socketUDP.Receive(receiveArray, 50);
		if (receiveAddress.GetPortRef() != NULL)
		{
			ReceiveMessageClient();
			receiveAddress = NetAddress();
		}
	}
}

void SendRequestClient(int messageType, NetSocketUDP socketUDP)
{
	ClearReceiveArray();

	switch (messageType)
	{
	case 0:
		Print("\nNickname: ");
		std::cin >> charInput;

		for (int i = 2; i < 22; ++i)
		{
			sendArray[i] = charInput[i - 2];
		}

		Println("\nRound Settings: ");
		Println("0: Any");
		Println("1: Best of 1");
		Println("2: Best of 3");
		Println("3: Best of 5");
		Print("Your Action: ");
		std::cin >> input;
		Println("");

		if (input != 0 && input != 1 && input != 2 && input != 3)
		{
			Println("UNKOWN INPUT: Message canceled!");
			return;
		}
		sendArray[0] = 0;
		sendArray[1] = input << 5;

		Println("\nTime Settings: ");
		Println("0: Any");
		Println("1: 60 seconds");
		Println("2: 90 seconds");
		Println("3: 120 seconds");
		Print("Your Action: ");
		std::cin >> input;
		Println("");

		if (input != 0 && input != 1 && input != 2 && input != 3)
		{
			Println("UNKOWN INPUT: Message canceled!\n");
			return;
		}
		sendArray[1] |= input << 2;

		socketUDP.Send(serverAddress, (char*)sendArray, 22).m_errorCode;
		//Println(socketUDP.Send(serverAddress, (char*)sendArray, 22).m_errorCode);
		break;









	case 2:
		Print("\nNickname: ");
		std::cin >> charInput;

		for (int i = 2; i < 22; ++i)
		{
			sendArray[i] = charInput[i - 2];
		}

		Println("\nRound Settings: ");
		Println("1: Best of 1");
		Println("2: Best of 3");
		Println("3: Best of 5");
		Print("Your Action: ");
		std::cin >> input;
		if (input != 1 && input != 2 && input != 3)
		{
			Println("UNKOWN INPUT: Message canceled!");
			return;
		}
		sendArray[0] = 2 << 1;
		sendArray[1] = input << 5;

		Println("\nTime Settings: ");
		Println("1: 60 seconds");
		Println("2: 90 seconds");
		Println("3: 120 seconds");
		Print("Your Action: ");
		std::cin >> input;
		Println("");
		if (input != 1 && input != 2 && input != 3)
		{
			Println("UNKOWN INPUT: Message canceled!\n");
			return;
		}
		sendArray[1] |= input << 2;

		socketUDP.Send(serverAddress, (char*)sendArray, 22).m_errorCode;
		//Println(socketUDP.Send(serverAddress, (char*)sendArray, 22).m_errorCode);
		break;





	case 3:
		if (myRoomID >= 0)
		{
			sendArray[0] = 3 << 1;
			sendArray[1] = myRoomID;

			Println("Request to leave your room (ID: " << (int)myRoomID << ") was send");
			socketUDP.Send(serverAddress, (char*)sendArray, 2).m_errorCode;
			//Println(socketUDP.Send(serverAddress, (char*)sendArray, 2).m_errorCode);
			break;
		}
		Println("You are in no room");
		break;
	default:

		Println("UNKOWN COMMAND");
		break;
	}
}
void ReceiveMessageClient()
{
	identifier = receiveArray[0] >> 1;
	status = receiveArray[0] << 7;
	status = status >> 7;


	switch (identifier)
	{
	case 0:
		if (status)
		{
			Print("Joined Room with ID: ");
			Println((int)receiveArray[1]);

			Print("Rival: ");
			for (int i = 0; (i < 20) && (receiveArray[i + 2] != -52); i++)
			{
				opponentName[i] = receiveArray[i + 2];
				Print(receiveArray[i + 2]);
			}
			Println("");

			myRoomID = receiveArray[1];
		}
		else
		{
			Println("Room creation failed");
		}
		break;

	case 1:
		Print("Player: ");
		for (int i = 0; (i < 20) && (receiveArray[i + 1] != -52); i++)
		{
			opponentName[i] = receiveArray[i + 1];
			Print(receiveArray[i + 1]);
		}
		Println(" joined your room");
		break;

	case 2:
		if (status)
		{
			Print("Created Room with ID: ");
			Println((int)receiveArray[1]);
			myRoomID = receiveArray[1];
		}
		else
		{
			Println("Room creation failed");
		}
		break;
	case 3:
		if (status)
		{
			Println("You left your Room with ID: " << (int)myRoomID);
			myRoomID = NULL;
		}
		else
		{
			Println("Room leave request failed");
		}
		break;
	case 4:
		Println("");
		Print("The Player: ");
		for (int i = 0; (i < 20) && (opponentName[i] != 0); i++)
			Print(opponentName[i]);

		Println(" left the room");
		break;
	default:
		myRoomID = NULL;
		Println("UNKOWN COMMAND")
			break;
	}

	SendReceiveMessageClient();
}
void SendReceiveMessageClient()
{
	sendArray[0] = receiveArray[0] >> 1;
	sendArray[0] = sendArray[0] << 1;
	sendArray[0] |= static_cast<char>(1);
	socketUDP.Send(serverAddress, (char*)sendArray, 1).m_errorCode;
	//Print("ErrorCode of sending receive message: ");
	//Println(socketUDP.Send(serverAddress, (char*)sendArray, 1).m_errorCode);

	Println("");
	Println("0: Room request");
	Println("2: Create room");
	Println("3: Leave room");
	Println("");
	Print("Your Action: ");
}

void ClearReceiveArray()
{
	int a = 21;
	for (int i = 0; i < a; ++i)
	{
		charInput[i] = 0;
	}

	a = sizeof(sendArray) / sizeof(*sendArray);
	for (int i = 0; i < a; ++i)
	{
		receiveArray[i] = 0;
		sendArray[i] = 0;
	}
}