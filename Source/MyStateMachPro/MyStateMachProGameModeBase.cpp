// Fill out your copyright notice in the Description page of Project Settings.

#include "MyStateMachProGameModeBase.h"
#include "MyStateMachPro.h"
#include "UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMyStateMachProGameModeBase::StartPlay() {

	Super::StartPlay();
	MyMainMenu = CreateWidget<UUserWidget>(GetWorld(), wMainMenu);

	MyMainMenu->AddToViewport();


}
