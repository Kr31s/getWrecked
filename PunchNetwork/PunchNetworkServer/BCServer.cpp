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
	delete(serverRunning);
	delete(totalRoomID);
	delete(totalRoomID);
	delete(messageListe);
}

void BCServer::SendDataBCM(BCClient * receiver, SendType p_status, char* dataArray, unsigned int lengthArrayToSend)
{
	messageListe->push_back(*new BCMessage(*receiver, GetTimeInMilli(), dataArray, lengthArrayToSend));

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
	ErrorCheck(serverSocket.Send(receiver->m_netaddress, (char*)dataArray, lengthArrayToSend).m_errorCode);
#else
	serverSocket.Send(receiver->m_netaddress, (char*)dataArray, lengthArrayToSend);
#endif
}

void BCServer::SendData(NetAddress & netAddress, SendType p_status, char* dataArray, unsigned int lengthArrayToSend)
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
	ErrorCheck(serverSocket.Send(receiver, (char*)dataArray, lengthArrayToSend).m_errorCode);
#else
	serverSocket.Send(netAddress, (char*)dataArray, lengthArrayToSend);
#endif
}
