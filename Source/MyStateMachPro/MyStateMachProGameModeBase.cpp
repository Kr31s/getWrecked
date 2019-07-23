// Fill out your copyright notice in the Description page of Project Settings.

#include "MyStateMachProGameModeBase.h"
#include "MyStateMachPro.h"
#include "UserWidget.h"
#include "Public/MyCameraActor.h"
#include "Kismet/GameplayStatics.h"

unsigned int AMyStateMachProGameModeBase::sFrameCounter = 1;

AMyStateMachProGameModeBase::AMyStateMachProGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	roundTimer = roundTime;
	if (NetworkSystem::NetSys != nullptr)
	{
		NetworkSystem::NetSys->setGameMode(this);
		GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("Alive2"));
	}

	//startTimer = 3.0F;
	MatchCount = EMatcheTypes::BestofOne;
	transitionMaxDuration = 3.0F;
	transitionSpeed = 0.1F;
}


void AMyStateMachProGameModeBase::BeginDestroy() {
	Super::BeginDestroy();
	GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("startdown"));
	AMyStateMachProGameModeBase::sFrameCounter = 1;
	if (NetworkSystem::NetSys != nullptr) {

		NetworkSystem::NetSys->ShutdownNetwork();
		GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("down"));
	}
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
	if (APlayerController * PC = UGameplayStatics::CreatePlayer(this, -1))
	{
		if (APawn * Pawn = PC->GetPawn())
		{
			// Move this player forward (hardcoded distance, should be a UPROPERTY) and then turn around.
			Pawn->SetActorLocation(FVector(230, 0.0F, 100.0F));
			//Pawn->SetActorLocation(Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 250.0f);
			Pawn->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}

	player1 = Cast<AFGDefaultPawn>(UGameplayStatics::GetPlayerCharacter(this, 0));
	player2 = Cast<AFGDefaultPawn>(UGameplayStatics::GetPlayerCharacter(this, 1));

	player1->Opponent = UGameplayStatics::GetPlayerCharacter(this, 1);
	player2->Opponent = UGameplayStatics::GetPlayerCharacter(this, 0);
	player1->master = true;
	player1->isOnLeftSide = true;
	player2->isOnLeftSide = false;
	startTimer = prepTime;
	player1Score = 0;
	player2Score = 0;
	roundNumber = 1;

}
void AMyStateMachProGameModeBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if (NetworkSystem::NetSys != nullptr && player1 != nullptr)
	{
		NetworkSystem::NetSys->GameMessage(player1->SendInputStream);
		++AMyStateMachProGameModeBase::sFrameCounter;

		player2->DoMovesFromInputStream(std::bitset<12>(NetworkSystem::NetSys->gameMessagesRivale[0].m_input));
		/*for(int i = 0; i<9;++i)
		{
			if (NetworkSystem::NetSys->gameMessagesRivale[i].m_time == AMyStateMachProGameModeBase::sFrameCounter) {
				player2->DoMovesFromInputStream(std::bitset<12>(NetworkSystem::NetSys->gameMessagesRivale[i].m_time));
			}
			break;
		}
				UE_LOG(LogTemp, Warning, TEXT("error"));*/
	}
	if (startTimer == 3.0f) {

		DisableInput(Cast<APlayerController>(player1));
		DisableInput(Cast<APlayerController>(player2));
		player1->AddMovementInput(player1->GetActorForwardVector(), 0.0F);
		player2->AddMovementInput(player2->GetActorForwardVector(), 0.0F);
		player1->doJump = false;
		player2->doJump = false;

		player1->isStunned = false;
		player2->isStunned = false;
		if (NetworkSystem::NetSys != nullptr && NetworkSystem::NetSys->roomOwner)
		{
			player1->SetActorLocation(FVector(-230, 0.0F, 100.0F));
			player2->SetActorLocation(FVector(230, 0.0F, 100.0F));
			player1->isOnLeftSide = true;
			player2->isOnLeftSide = false;
		}
		else {

			player2->SetActorLocation(FVector(-230, 0.0F, 100.0F));
			player1->SetActorLocation(FVector(230, 0.0F, 100.0F));
			player2->isOnLeftSide = true;
			player1->isOnLeftSide = false;
		}
		//player1->GetMesh()->SetRelativeScale3D(FVector(1.0F, -1.0F, 1.0F));
		//player2->GetMesh()->SetRelativeScale3D(FVector(1.0F, 1.0F, 1.0F));
		roundTimer = roundTime;
		player1->SetDirectionInputX(0.0F);
		player2->SetDirectionInputX(0.0F);
	}

	if (startTimer > 0.0f) 
	{
		startTimer -= DeltaSeconds;
		GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Magenta, FString::SanitizeFloat(startTimer));
		return;
	}

	
	//roundTimer -= DeltaSeconds;
	SetRoundTimer(DeltaSeconds);
	CheckOnWhichSidePlayerIs();

	if (player1->RessourceComp->Health <= 0.0F)
	{
		//player1->K2_DestroyActor();
		GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("Player2Wins"));
		//UGameplayStatics::SetGamePaused(this, true);
		if(!scoreFlag)
		{
			player2Score++;
			scoreFlag = true;
		}
		OnP2ScoreChanged.Broadcast(player2Score);

			
		while (transitiontime < transitionMaxDuration) {
				transitiontime += DeltaSeconds;
				//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Yellow, TEXT("WHILE-TRANSITION"));
				player1->CustomTimeDilation = transitionSpeed;
				player2->CustomTimeDilation = transitionSpeed;
				return;
		}

		SetupMatch();
		DetermineMatchWinner();
		transitiontime = 0;
	}
	if (player2->RessourceComp->Health <= 0.0F)
	{
		//player2->K2_DestroyActor();
		//GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("Player1Wins"));
		//UGameplayStatics::SetGamePaused(this, true);
		if (!scoreFlag)
		{
			player1Score++;
			scoreFlag = true;
		}
		OnP1ScoreChanged.Broadcast(player1Score);
			
		while (transitiontime < transitionMaxDuration) {
			transitiontime += DeltaSeconds;
			player1->CustomTimeDilation = transitionSpeed;
			player2->CustomTimeDilation = transitionSpeed;
			return;
		} 

		SetupMatch();
		DetermineMatchWinner();
		transitiontime = 0;
	}
	if (roundTimer <= 0.0F)
	{
		RoundTimeOver();
	}
}

void AMyStateMachProGameModeBase::SetupMatch()
{
	scoreFlag = false;
	startTimer = prepTime;
	roundTimer = roundTime;
	player1->CustomTimeDilation = 1.0F;
	player2->CustomTimeDilation = 1.0F;

	player1->RessourceComp->SetHealth(1.0F);
	player2->RessourceComp->SetHealth(1.0F);
	player1->RessourceComp->SetStunMeter(0.0F);
	player2->RessourceComp->SetStunMeter(0.0F);
	player1->RessourceComp->SetPowerMeter(0.0F);
	player2->RessourceComp->SetPowerMeter(0.0F);
	//Reset UI Healthbar
//	player1->RessourceComp->OnHealthChanged.Broadcast(player1, player1->RessourceComp->Health);
//	player2->RessourceComp->OnHealthChanged.Broadcast(player2, player2->RessourceComp->Health);
//
//	//Reset UI StunBar
//	player1->RessourceComp->OnStunMeterChanged.Broadcast(player1, player1->RessourceComp->StunMeter);
//	player2->RessourceComp->OnStunMeterChanged.Broadcast(player2, player2->RessourceComp->StunMeter);
	// position need to be changable values in the engine 
	player1->SetActorLocation(FVector(-230, 0.0F, 100.0F));
	player2->SetActorLocation(FVector(230, 0.0F, 100.0F));
	++roundNumber;
	OnMatchNumberChanged.Broadcast(roundNumber);
}

void AMyStateMachProGameModeBase::CheckOnWhichSidePlayerIs()
{
	if (player1->GetActorLocation().X < player2->GetActorLocation().X)
	{
		if (player1->GetCharacterMovement()->IsMovingOnGround())
		{
			player1->isOnLeftSide = true;
		}
		if (player2->GetCharacterMovement()->IsMovingOnGround())
		{
			player2->isOnLeftSide = false;
		}
	}
	else
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
}

void AMyStateMachProGameModeBase::DetermineMatchWinner()
{
	switch (MatchCount)
	{
	case EMatcheTypes::BestofOne:
		if (player1Score == 1)
		{
			//player 1 wins Complete Match
		}
		if (player2Score == 1)
		{
			//player 2 wins Complete Match
		}
		break;
	case EMatcheTypes::BestofThree:
		if (player1Score == 3)
		{
			//player 1 wins Complete Match
		}
		if (player2Score == 3)
		{
			//player 2 wins Complete Match
		}
		break;
	case EMatcheTypes::BestofFive:
		if (player1Score == 5)
		{
			//player 1 wins Complete Match
		}
		if (player2Score == 5)
		{
			//player 2 wins Complete Match
		}
		break;
	default:
		break;
	}
}

void AMyStateMachProGameModeBase::SetRoundTimer(float deltaSeconds)
{
	roundTimer -= deltaSeconds;
	OnTimeChanged.Broadcast(this->roundTimer);
}

void AMyStateMachProGameModeBase::RoundTimeOver()
{
	if (player1->RessourceComp->Health > player2->RessourceComp->Health)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("Player1Wins"));
		//UGameplayStatics::SetGamePaused(this, true);
		player1Score++;
		OnP1ScoreChanged.Broadcast(player1Score);
		SetupMatch();
		DetermineMatchWinner();
	}
	else
	{
		//player2->K2_DestroyActor();
		GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("Player2Wins"));
		//UGameplayStatics::SetGamePaused(this, true);
		player2Score++;
		OnP2ScoreChanged.Broadcast(player2Score);
		SetupMatch();
		DetermineMatchWinner();
	}
}
