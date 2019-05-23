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
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	

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

	player1 = Cast<AFGDefaultPawn>(UGameplayStatics::GetPlayerCharacter(this, 0));
	player2 = Cast<AFGDefaultPawn>(UGameplayStatics::GetPlayerCharacter(this, 1));

	player1->Opponent = UGameplayStatics::GetPlayerCharacter(this, 1);
	player2->Opponent = UGameplayStatics::GetPlayerCharacter(this, 0);
	UE_LOG(LogTemp, Warning, TEXT("No initial move."));
	/*GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, FString::FromInt(GetWorld()->GetNumPlayerControllers()));
	GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("player1"));

	GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Yellow, FString::FromInt(GetWorld()->GetNumPlayerControllers()));
	GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Yellow, TEXT("player2"));
	// GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Yellow, Opponent->GetName());
	*/
	player1->isOnLeftSide = true;
	player2->isOnLeftSide = false;
	//UGameplayStatics::SetGamePaused(this, true);
}
void AMyStateMachProGameModeBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if(player1->GetActorLocation().X < player2->GetActorLocation().X)
	{
		player1->isOnLeftSide = true;
		player2->isOnLeftSide = false;
	}else
	{
		player1->isOnLeftSide = false;
		player2->isOnLeftSide = true;
	}
		GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Yellow, TEXT("GMisTicking"));
}
