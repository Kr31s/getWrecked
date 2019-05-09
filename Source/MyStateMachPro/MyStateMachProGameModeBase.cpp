// Fill out your copyright notice in the Description page of Project Settings.

#include "MyStateMachPro.h"
#include "Kismet/GameplayStatics.h"
#include "MyStateMachProGameModeBase.h"

void AMyStateMachProGameModeBase::StartPlay() {

	Super::StartPlay();
MyMainMenu = CreateWidget<UUserWidget>(this, wMainMenu);

MyMainMenu->AddToViewport();
}
