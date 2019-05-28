// Fill out your copyright notice in the Description page of Project Settings.

#include "MyStateMachProGameModeBase.h"
#include "MyStateMachPro.h"
#include "UserWidget.h"
#include "Public/MyCameraActor.h"
#include "Kismet/GameplayStatics.h"

AMyStateMachProGameModeBase::AMyStateMachProGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	roundTimer = 99.0F;
	//startTimer = 3.0F;

}

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

	player1 = Cast<AFGDefaultPawn>(UGameplayStatics::GetPlayerCharacter(this, 0));
	player2 = Cast<AFGDefaultPawn>(UGameplayStatics::GetPlayerCharacter(this, 1));

	player1->Opponent = UGameplayStatics::GetPlayerCharacter(this, 1);
	player2->Opponent = UGameplayStatics::GetPlayerCharacter(this, 0);
	UE_LOG(LogTemp, Warning, TEXT("No initial move."));

	player1->isOnLeftSide = true;
	player2->isOnLeftSide = false;
	startTimer = 3.0F;

}
void AMyStateMachProGameModeBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if(startTimer >= 0){
		startTimer -= DeltaSeconds;
		player1->isStunned = true;
		player2->isStunned = true;
		GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Magenta, FString::SanitizeFloat(startTimer));
		return;
	}
	player1->isStunned = false;
	player2->isStunned = false;
	roundTimer -= DeltaSeconds;
	if(player1->GetActorLocation().X < player2->GetActorLocation().X)
	{
		if(player1->GetCharacterMovement()->IsMovingOnGround())
		{
			player1->isOnLeftSide = true;
		}
		if (player2->GetCharacterMovement()->IsMovingOnGround())
		{
			player2->isOnLeftSide = false;
		}
	}else
	{
		if (player1->GetCharacterMovement()->IsMovingOnGround())
		{
			player1->isOnLeftSide = false;
		}
		if (player2->GetCharacterMovement()->IsMovingOnGround())
		{
			player2->isOnLeftSide = true;
		}
	}
	if(player1->RessourceComp->Health <= 0.0F)
	{
		//player1->K2_DestroyActor();
		GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("Player2Wins"));
		//UGameplayStatics::SetGamePaused(this, true);
		player1->RessourceComp->SetHealth(1.0F);
		player2->RessourceComp->SetHealth(1.0F);
		player1->RessourceComp->SetStunMeter(0.0F);
		player2->RessourceComp->SetStunMeter(0.0F);
		player1->SetActorLocation(FVector(-230, 0.0F, 100.0F));
		player2->SetActorLocation(FVector(230, 0.0F, 100.0F));
	}
	if(player2->RessourceComp->Health <= 0.0F)
	{
		//player2->K2_DestroyActor();
		GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("Player1Wins"));
		//UGameplayStatics::SetGamePaused(this, true);
		player1->RessourceComp->SetHealth(1.0F);
		player2->RessourceComp->SetHealth(1.0F);
		player1->RessourceComp->SetStunMeter(0.0F);
		player2->RessourceComp->SetStunMeter(0.0F);
		player1->SetActorLocation(FVector(-230, 0.0F, 100.0F));
		player2->SetActorLocation(FVector(230, 0.0F, 100.0F));
	}
}
