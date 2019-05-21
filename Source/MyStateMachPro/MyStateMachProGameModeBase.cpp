// Fill out your copyright notice in the Description page of Project Settings.

#include "MyStateMachProGameModeBase.h"
#include "MyStateMachPro.h"
#include "UserWidget.h"
#include "Public/MyCameraActor.h"
#include "Kismet/GameplayStatics.h"

void AMyStateMachProGameModeBase::StartPlay() {

	Super::StartPlay();
	//MyMainMenu = CreateWidget<UUserWidget>(GetWorld(), wMainMenu);

	//MyMainMenu->AddToViewport();

	if (UWorld * World = GetWorld())
	{
		MainGameCamera = Cast<AMyCameraActor>(World->SpawnActor(AMyCameraActor::StaticClass(), &FTransform::Identity));
	}


	// Create another player
	if (APlayerController * PC = UGameplayStatics::CreatePlayer(this,-1))
	{
		if (APawn * Pawn = PC->GetPawn())
		{
			// Move this player forward (hardcoded distance, should be a UPROPERTY) and then turn around.
			Pawn->SetActorLocation(FVector(230,0.0F,100.0F));
			//Pawn->SetActorLocation(Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 250.0f);
			Pawn->SetActorRotation(FRotator(0.0f, 0.0f,0.0f));
		}
	}
}
void AMyStateMachProGameModeBase::Tick(float Deltaseconds) {
	Super::Tick(Deltaseconds);
}


