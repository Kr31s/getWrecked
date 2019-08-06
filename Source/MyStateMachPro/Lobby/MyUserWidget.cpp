// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "NetworkSystem.h"
#include "MyStateMachProGameModeBase.h"

UMyUserWidget* UMyUserWidget::myUserWidget = NULL;
bool UMyUserWidget::threadDestroyFlag = true;
void UMyUserWidget::DeactivateThreadDestroy()
{
	UMyUserWidget::threadDestroyFlag = false;
}
void UMyUserWidget::BeginDestroy()
{
	Super::BeginDestroy();
	UE_LOG(LogTemp, Warning, TEXT("threadDestroyFlag: %s"), (UMyUserWidget::threadDestroyFlag ? TEXT("True") : TEXT("False")));

	if (UMyUserWidget::threadDestroyFlag && NetworkSystem::NetSys != nullptr) {
		NetworkSystem::NetSys->ShutdownNetwork();
	}

	UMyUserWidget::myUserWidget = nullptr;
}

void UMyUserWidget::SetSingelton() {
	UMyUserWidget::myUserWidget = this;
}

void UMyUserWidget::CreateRoom(int p_timeValue, int p_roundValue, const FString& p_Name)
{
	AMyStateMachProGameModeBase::m_playerName = p_Name;
	AMyStateMachProGameModeBase::m_roundVal = p_roundValue;
	AMyStateMachProGameModeBase::m_timeVal = p_timeValue;

	NetworkSystem::NetSys->CreateRoom(p_timeValue, p_roundValue, p_Name);
}

void UMyUserWidget::JoinRoom(int p_timeValue, int p_roundValue, const FString& p_Name)
{
	AMyStateMachProGameModeBase::m_opponentName = p_Name;
	AMyStateMachProGameModeBase::m_roundVal = p_roundValue;
	AMyStateMachProGameModeBase::m_timeVal = p_timeValue;

	NetworkSystem::NetSys->RoomRequest(p_timeValue, p_roundValue, p_Name);
}

void UMyUserWidget::LeaveRoom()
{
	NetworkSystem::NetSys->LeaveRoom();
}

bool UMyUserWidget::CreateClient(int p_IP1, int p_IP2, int p_IP3, int p_IP4, int p_Port)
{
	UMyUserWidget::myUserWidget = this;

	if (NetworkSystem::NetSys == nullptr) {
		NetworkSystem::NetSys = new NetworkSystem();
	}

	UE_LOG(LogTemp, Warning, TEXT("CreateClient"));
	NetworkSystem::NetSys->serverAddress = NetAddress(p_IP1, p_IP2, p_IP3, p_IP4, p_Port);

	return NetworkSystem::NetSys->InitNetSystem();
}

bool UMyUserWidget::GetRoomFullStatus() 
{
	return NetworkSystem::roomFull;
}

void UMyUserWidget::ElementUpdateLobby(int slot1Pos, int slot2Pos, bool ready)
{
	NetworkSystem::NetSys->ElementChanged(slot1Pos, slot2Pos, ready);
}

void UMyUserWidget::SetGameValues(int p_timeValue, int p_roundValue, const FString& p_playerName, const FString& p_opponentName)
{
	AMyStateMachProGameModeBase::m_playerName = p_playerName;
	AMyStateMachProGameModeBase::m_opponentName = p_opponentName;
	AMyStateMachProGameModeBase::m_roundVal = p_roundValue;
	AMyStateMachProGameModeBase::m_timeVal = p_timeValue;
}
