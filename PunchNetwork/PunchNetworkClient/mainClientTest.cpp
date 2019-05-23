//#include "Inc_BWNetworking.h"
//#include "Inc_SmartMacros.h"
//#include "Inc_BWMath.h"
//
//#include <stdexcept>
//#include <thread>
//#include <chrono>
//
//void ClientMethod();
//void ServerMethod();
//unsigned int EncodeMessage(char identifier, char* messageArray);
//void DecodeMessage(char* messageArray);
//
//bool gameIsRunning = true;
//
//void main()
//{
//	std::thread t1(ServerMethod);
//
//	int i;
//	std::cin >> i;
//}
//
//void ClientMethod()
//{
//	NetSocketUDP udpSocket;
//	NetAddress address(93, 201, 86, 44, 4405);
//
//	unsigned char sendArray1[]{ "Netzwerk läuft woop woop woop woop woop woop woop" };
//	unsigned char sendArray2[]{ "Netzwerk läuft!!" };
//
//	NetPackage package(&address, sendArray2, sizeof(sendArray2));
//
//	Println(BWNet::InitializeSocketLayer().m_errorCode);
//	Println(udpSocket.OpenSocket(0).m_errorCode);
//	Println(udpSocket.Send(package).m_errorCode);
//}
//
//void ServerMethod()
//{
//	NetSocketUDP udpSocket;
//	unsigned char receiveArray[4];
//
//
//	Print("InitializeSocketLayer: ");
//	Println(BWNet::InitializeSocketLayer().m_errorCode);
//	Print("OpenSocket 4405: ");
//	Println(udpSocket.OpenSocket(4405).m_errorCode);
//	Print("Is Socket 4405 open ? ");
//	Println(((udpSocket.IsOpen() == 1) ? "true" : "false"));
//	Print("Enable Non-Blocking: ");
//	Println(udpSocket.EnableNonBlocking().m_errorCode);
//
//	ReceiveResult resultUDP;
//	int a = 0;
//	while (gameIsRunning)
//	{
//		resultUDP = udpSocket.Receive(receiveArray, 50);
//		if (resultUDP.GetPort() != NULL)
//		{
//			for (int i = 0; i < 50; ++i)
//				Print(receiveArray[i]);
//
//			DecodeMessage((char*)receiveArray);
//			//Println("");
//			//Println(resultUDP.GetAdress());
//			//Println(resultUDP.GetPort());
//		}
//		std::this_thread::sleep_for(std::chrono::milliseconds(16));
//	}
//}
//
//unsigned int EncodeMessage(char identifier, char* messageArray)
//{
//	switch (identifier) {
//
//	case 0://Ingame Message
//		return messageArray[1] | (messageArray[0] < 12) | (identifier < 25);
//		break;
//	default:
//		throw std::invalid_argument("The identifier is undefined");
//		break;
//
//	}
//	//get identifier
//}
//
//void DecodeMessage(char* messageArray)
//{
//	//get identifier
//	unsigned char identifier = messageArray[0] > 1;
//
//	Println("Identifier: " << identifier);
//
//	switch (identifier) {
//
//	case 0://Ingame Message
//		unsigned int timeFrame = (static_cast<unsigned int>(messageArray[0] < 7) < 5) | (static_cast<unsigned int>(messageArray[1] < 4)) | (static_cast<unsigned int>(messageArray[1] > 4));
//		Println("TimeFrame: " << timeFrame);
//		break;
//	}
//	//get identifier
//}