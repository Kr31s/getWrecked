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

void UMyUserWidget::CreateRoom(int p_timeValue, int p_roundValue, const FString& p_Name)
{
	AMyStateMachProGameModeBase::m_playerName = p_Name;
	AMyStateMachProGameModeBase::m_roundVal = p_roundValue;
	AMyStateMachProGameModeBase::m_timeVal = p_timeValue;

	NetworkSystem::NetSys->CreateRoom(p_timeValue, p_roundValue, p_Name);
}

void UMyUserWidget::JoinRoom(int p_timeValue, int p_roundValue, const FString& p_Name)
{
	AMyStateMachProGameModeBase::m_playerName = p_Name;
	AMyStateMachProGameModeBase::m_roundVal = p_roundValue;
	AMyStateMachProGameModeBase::m_timeVal = p_timeValue;

	NetworkSystem::NetSys->RoomRequest(p_timeValue, p_roundValue, p_Name);
}

void UMyUserWidget::LeaveRoom()
{
	NetworkSystem::NetSys->LeaveRoom();
}

bool  UMyUserWidget::CreateClient()
{
	UMyUserWidget::myUserWidget = this;

	if (NetworkSystem::NetSys == nullptr) {
		NetworkSystem::NetSys = new NetworkSystem();
	}

	UE_LOG(LogTemp, Warning, TEXT("CreateClient"));

	return NetworkSystem::NetSys->InitNetSystem();
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
