// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUIClass.h"
#include "NetworkSystem.h"
#include <RessourceData.cpp>

void UInGameUIClass::PauseGameUI_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Emerald, TEXT("test"));
}
