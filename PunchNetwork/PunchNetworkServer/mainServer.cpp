#include "ServerDefines.h"

#include "BCServer.h"
#include "BCRoom.h"
#include "BCClient.h"
#include "BCMessage.h"



void DecodeMessageServer(NetAddress& p_receiveAddress, char* p_receiveArray, unsigned char& p_rounds, unsigned char& p_gameTime, unsigned int& p_intValue);


void ServerThread()
{
	NetAddress receiveAddress;
	char	serverThreadArray[46] = { 0 };

	Messages		identifier = Messages::UnknownMessage;
	unsigned int	intValue = NULL;
	unsigned char	rounds = NULL;
	unsigned char	gameTime = NULL;
	unsigned char	status = NULL;


	while (BCServer::sTheServer->m_serverRunning)
	{
		receiveAddress = BCServer::sTheServer->m_serverSocket.Receive((char*)serverThreadArray, sizeof(serverThreadArray));
		if (receiveAddress.GetPortRef() != NULL)
		{
			DecodeMessageServer(receiveAddress, serverThreadArray, rounds, gameTime, intValue);

			ClearReceiveArray(serverThreadArray, sizeof(serverThreadArray));
			intValue = NULL;
			rounds = NULL;
			gameTime = NULL;
			identifier = Messages::UnknownMessage;
			status = NULL;
		}
	}

	Println("ServerThread closed");
}
void MessageThread()
{
	char	messageThreadArray[46] = { 0 };

	while (BCServer::sTheServer->m_serverRunning)
	{
		BCMessage::CheckResendMessages(messageThreadArray);
	}
	Println("MessageThread closed");
}
void HeartThread()
{
	char heartThreadArray[46] = { 0 };
	heartThreadArray[0] = 5;

	while (BCServer::sTheServer->m_serverRunning)
	{
		sMutexClientIDList.lock();
		for (int i = 0; i < BCServer::sTheServer->m_clientIDList->size(); ++i)
		{
			if (BCServer::sTheServer->m_clientIDList->at(i).m_clientStatus != ClientStatus::Offline)
			{
				BCServer::sTheServer->SendDataBCM(BCServer::sTheServer->m_clientIDList->at(i).m_clientID, SendType::False, heartThreadArray);
			}
		}
		sMutexClientIDList.unlock();

		Println("Sleep 2sec");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	Println("HeartThread closed");
}

void NonServerMessage()
{

}

void DecodeMessageServer(NetAddress& p_receiveAddress, char* p_receiveArray, unsigned char& p_rounds, unsigned char& p_gameTime, unsigned int& p_intValue)
{
	if (p_receiveArray[1] == 0)
	{
		switch (MessageOfIndex(p_receiveArray[0]))
		{
		case Messages::RoomRequest:
			BCServer::sTheServer->RoomRequest(p_receiveAddress, p_receiveArray, p_rounds, p_gameTime);
			break;
		case Messages::CreateRoom:
			BCServer::sTheServer->CreateRoom(p_receiveAddress, p_receiveArray, p_rounds, p_gameTime);
			break;
		case Messages::LeaveRoom:
			BCServer::sTheServer->LeaveRoom(p_receiveAddress, p_receiveArray);
			break;
		case Messages::ElementChange:
			BCServer::sTheServer->ElementChange(p_receiveAddress, p_receiveArray);
			break;
		case Messages::PauseGame:
			BCServer::sTheServer->PauseGame(p_receiveAddress, p_receiveArray);
			break;
		case Messages::GameMessage:
			BCServer::sTheServer->GameMessage(p_receiveAddress, p_receiveArray, p_intValue);
			break;


		default:
			NonServerMessage();
			break;
		}
	}
	else
	{
		BCMessage::GetReplyMessage((int)p_receiveArray[45]);
	}
}

int main()
{
	int values1[]{  7,3,1,6,7,1,7,6,5,3,1,3,3,0,6,2,4,9,1,9,2,2,5,1,1,9,6,7,4,4,2,6,5,7,4,7,4,2,3,5,5,3,4,9,1,9,4,9,3,4 };
	int values2[]{  9,6,9,8,3,5,2,0,3,1,2,7,7,4,5,0,6,3,2,6,2,3,9,5,7,8,3,1,8,0,1,6,9,8,4,8,0,1,8,6,9,4,7,8,8,5,1,8,4,3 };
	int values3[]{  8,5,8,6,1,5,6,0,7,8,9,1,1,2,9,4,9,4,9,5,4,5,9,5,0,1,7,3,7,9,5,8,3,3,1,9,5,2,8,5,3,2,0,8,8,0,5,5,1,1 };
	int values4[]{  1,2,5,4,0,6,9,8,7,4,7,1,5,8,5,2,3,8,6,3,0,5,0,7,1,5,6,9,3,2,9,0,9,6,3,2,9,5,2,2,7,4,4,3,0,4,3,5,5,7 };
	int values5[]{  6,6,8,9,6,6,4,8,9,5,0,4,4,5,2,4,4,5,2,3,1,6,1,7,3,1,8,5,6,4,0,3,0,9,8,7,1,1,1,2,1,7,2,2,3,8,3,1,1,3 };
	int values6[]{  6,2,2,2,9,8,9,3,4,2,3,3,8,0,3,0,8,1,3,5,3,3,6,2,7,6,6,1,4,2,8,2,8,0,6,4,4,4,4,8,6,6,4,5,2,3,8,7,4,9 };
	int values7[]{  3,0,3,5,8,9,0,7,2,9,6,2,9,0,4,9,1,5,6,0,4,4,0,7,7,2,3,9,0,7,1,3,8,1,0,5,1,5,8,5,9,3,0,7,9,6,0,8,6,6 };
	int values8[]{  7,0,1,7,2,4,2,7,1,2,1,8,8,3,9,9,8,7,9,7,9,0,8,7,9,2,2,7,4,9,2,1,9,0,1,6,9,9,7,2,0,8,8,8,0,9,3,7,7,6 };
	int values9[]{  6,5,7,2,7,3,3,3,0,0,1,0,5,3,3,6,7,8,8,1,2,2,0,2,3,5,4,2,1,8,0,9,7,5,1,2,5,4,5,4,0,5,9,4,7,5,2,2,4,3 };
	int values10[]{ 5,2,5,8,4,9,0,7,7,1,1,6,7,0,5,5,6,0,1,3,6,0,4,8,3,9,5,8,6,4,4,6,7,0,6,3,2,4,4,1,5,7,2,2,1,5,5,3,9,7 };
	int values11[]{ 5,3,6,9,7,8,1,7,9,7,7,8,4,6,1,7,4,0,6,4,9,5,5,1,4,9,2,9,0,8,6,2,5,6,9,3,2,1,9,7,8,4,6,8,6,2,2,4,8,2 };
	int values12[]{ 8,3,9,7,2,2,4,1,3,7,5,6,5,7,0,5,6,0,5,7,4,9,0,2,6,1,4,0,7,9,7,2,9,6,8,6,5,2,4,1,4,5,3,5,1,0,0,4,7,4 };
	int values13[]{ 8,2,1,6,6,3,7,0,4,8,4,4,0,3,1,9,9,8,9,0,0,0,8,8,9,5,2,4,3,4,5,0,6,5,8,5,4,1,2,2,7,5,8,8,6,6,6,8,8,1 };
	int values14[]{ 1,6,4,2,7,1,7,1,4,7,9,9,2,4,4,4,2,9,2,8,2,3,0,8,6,3,4,6,5,6,7,4,8,1,3,9,1,9,1,2,3,1,6,2,8,2,4,5,8,6 };
	int values15[]{ 1,7,8,6,6,4,5,8,3,5,9,1,2,4,5,6,6,5,2,9,4,7,6,5,4,5,6,8,2,8,4,8,9,1,2,8,8,3,1,4,2,6,0,7,6,9,0,0,4,2 };
	int values16[]{ 2,4,2,1,9,0,2,2,6,7,1,0,5,5,6,2,6,3,2,1,1,1,1,1,0,9,3,7,0,5,4,4,2,1,7,5,0,6,9,4,1,6,5,8,9,6,0,4,0,8 };
	int values17[]{ 0,7,1,9,8,4,0,3,8,5,0,9,6,2,4,5,5,4,4,4,3,6,2,9,8,1,2,3,0,9,8,7,8,7,9,9,2,7,2,4,4,2,8,4,9,0,9,1,8,8 };
	int values18[]{ 8,4,5,8,0,1,5,6,1,6,6,0,9,7,9,1,9,1,3,3,8,7,5,4,9,9,2,0,0,5,2,4,0,6,3,6,8,9,9,1,2,5,6,0,7,1,7,6,0,6 };
	int values19[]{ 0,5,8,8,6,1,1,6,4,6,7,1,0,9,4,0,5,0,7,7,5,4,1,0,0,2,2,5,6,9,8,3,1,5,5,2,0,0,0,5,5,9,3,5,7,2,9,7,2,5 };
	int values20[]{ 7,1,6,3,6,2,6,9,5,6,1,8,8,2,6,7,0,4,2,8,2,5,2,4,8,3,6,0,0,8,2,3,2,5,7,5,3,0,4,2,0,7,5,2,9,6,3,4,5,0 };
	long long Valuetemp = 1;
	long long maxValue = 1;

	for (int ii = 0; ii < 37; ++ii) {

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values1[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;
		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values2[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values3[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values4[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values5[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values6[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values7[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values8[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values9[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values10[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values11[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values12[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values13[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values14[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values15[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values16[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values17[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values18[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;

		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values19[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

		for (int i = 0; i < 13; ++i)
		{
			Valuetemp *= values20[i+ii];
		}
		if (Valuetemp > maxValue) {
			maxValue = Valuetemp;
		}
		Valuetemp = 1;

	}

	std::cout << maxValue;

	BCServer::sTheServer = new BCServer(4405, true);

	std::thread t1(ServerThread);
	std::thread t2(MessageThread);
	std::thread t3(HeartThread);

	int i;
	std::cin >> i;

	BCServer::sTheServer->m_serverRunning = false;

	t1.join();
	t2.join();
	t3.join();

	return 0;
}
