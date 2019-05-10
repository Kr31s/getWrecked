// Fill out your copyright notice in the Description page of Project Settings.

#include "MyStateMachProGameModeBase.h"
<<<<<<< HEAD
#include "MyStateMachPro.h"
#include "UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMyStateMachProGameModeBase::StartPlay() {

	Super::StartPlay();
	MyMainMenu = CreateWidget<UUserWidget>(GetWorld(), wMainMenu);

	MyMainMenu->AddToViewport();


}
=======

>>>>>>> parent of adf3c0f... Animation Graph, GUI HP+Stun Bar
