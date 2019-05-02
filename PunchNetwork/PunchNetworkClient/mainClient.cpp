#include "Inc_BWNetworking.h"
#include "Inc_SmartMacros.h"
#include "Inc_BWMath.h"

#include <thread>
#include <bitset>

void ReceiveThread();
void SendRequestClient(int messageType, NetSocketUDP socketUDP);
void ReceiveMessageClient();
void SendReceiveMessageClient();


NetSocketUDP socketUDP;
NetAddress serverAddress(127, 0, 0, 1, 4405);
NetAddress receiveAddress;
unsigned char sendArray[50];
char receiveArray[50];

unsigned char identifier = NULL;
unsigned char status = NULL;
int input;
char* charInput = new char[20];

void main()
{

	BWNet::InitializeSocketLayer();

	Print("ErrorCode OpenSocket ");
	Println(socketUDP.OpenSocket(4000).m_errorCode);
	Println("");
	//socketUDP.EnableNonBlocking();

	std::thread t1(ReceiveThread);

	Println("0: Room request");
	Println("2: Create room");
	Println("3: Leave room");
	Println("");
	Print("Your Action: ");
	do
	{
		std::cin >> input;
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
	switch (messageType)
	{
	case 0:
		Print("\nNickname: ");
		std::cin >> charInput;

		for(int i = 2; i < 22; ++i)
		{
			sendArray[i] = charInput[i-2];
		}
		

		Println("\nRound Settings: ");
		Println("1: Best of 1");
		Println("2: Best of 3");
		Println("3: Best of 5");
		Print("Your Action: ")
			std::cin >> input;
		if (input != 1 && input != 2 && input != 3)
		{
			Println("UNKOWN INPUT: Message canceled!");
			return;
		}
		sendArray[0] = 0;
		sendArray[1] = input << 5;

		Println("\nTime Settings: ");
		Println("1: Best of 60");
		Println("2: Best of 90");
		Println("3: Best of 120");
		Print("Your Action: ")
			std::cin >> input;
		if (input != 1 && input != 2 && input != 3)
		{
			Println("UNKOWN INPUT: Message canceled!\n");
			return;
		}
		sendArray[1] |= input << 2;

		Println(socketUDP.Send(serverAddress, (char*)sendArray, 22).m_errorCode);
		break;









	case 2:
		Println("\nRound Settings: ");
		Println("1: Best of 1");
		Println("2: Best of 3");
		Println("3: Best of 5");
		Print("Your Action: ")
			std::cin >> input;
		if (input != 1 && input != 2 && input != 3)
		{
			Println("UNKOWN INPUT: Message canceled!");
			return;
		}
		sendArray[0] = 2 << 1;
		sendArray[1] = input << 5;

		Println("\nTime Settings: ");
		Println("1: Best of 60");
		Println("2: Best of 90");
		Println("3: Best of 120");
		Print("Your Action: ")
			std::cin >> input;
		if (input != 1 && input != 2 && input != 3)
		{
			Println("UNKOWN INPUT: Message canceled!\n");
			return;
		}
		sendArray[1] |= input << 2;

		Println(socketUDP.Send(serverAddress, (char*)sendArray, 2).m_errorCode);
		break;





	case 3:
		break;
	default:
		Println("UNKOWN COMMAND")
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

		break;
	case 1:
		break;
	case 2:
		if (status)
		{
			Print("Created Room with ID: ");
			Println((int)receiveArray[1]);
		}
		else
		{
			Print("Room creation failed");
		}

		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;

	default:
		Println("UNKOWN COMMAND")
			break;
	}

	SendReceiveMessageClient();
}
void SendReceiveMessageClient()
{
	sendArray[0] = receiveArray[0];
	Print("ErrorCode of sending receive message: ")
		Println(socketUDP.Send(serverAddress, (char*)sendArray, 1).m_errorCode);

	Println("");
	Println("0: Room request");
	Println("1: Room join");
	Println("2: Create room");
	Println("3: Leave room");
	Println("4: Opponent left room");
	Println("");
	Print("Your Action: ");
}
