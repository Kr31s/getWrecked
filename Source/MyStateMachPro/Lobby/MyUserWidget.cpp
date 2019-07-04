// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "NetworkSystem.h"

UMyUserWidget* UMyUserWidget::myUserWidget = NULL;

void UMyUserWidget::BeginDestroy()
{
	Super::BeginDestroy();
	FMessageReceiveThread::threadRuning = false;
	NetworkSystem::NetSys->MessageReceiveThread->Shutdown();
}

bool UMyUserWidget::CreateRoom(int p_timeValue, int p_roundValue, const FString& p_Name)
{
	NetworkSystem::NetSys->CreateRoom(p_timeValue, p_roundValue, p_Name);
	return true;
}

bool UMyUserWidget::JoinRoom(int p_timeValue, int p_roundValue, const FString& p_Name)
{
	NetworkSystem::NetSys->RoomRequest(p_timeValue, p_roundValue, p_Name);
	return true;
}

bool UMyUserWidget::LeaveRoom()
{
	NetworkSystem::NetSys->LeaveRoom();
	return true;
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

bool UMyUserWidget::ElementUpdateLobby(int slot1Pos, int slot2Pos, bool ready)
{
	NetworkSystem::NetSys->ElementChanged(slot1Pos, slot2Pos, ready);
	return true;
}
