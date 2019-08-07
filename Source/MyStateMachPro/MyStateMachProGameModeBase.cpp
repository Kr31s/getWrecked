// Fill out your copyright notice in the Description page of Project Settings.

#include "MyStateMachProGameModeBase.h"
#include "MyStateMachPro.h"
#include "UserWidget.h"
#include "Public/MyCameraActor.h"
#include "Kismet/GameplayStatics.h"

int AMyStateMachProGameModeBase::sFrameCounter = 1;

int AMyStateMachProGameModeBase::m_roundVal = 1;
int AMyStateMachProGameModeBase::m_timeVal = 1;
FString AMyStateMachProGameModeBase::m_opponentName = "Player2";
FString AMyStateMachProGameModeBase::m_playerName = "Player1";

bool AMyStateMachProGameModeBase::hasGameStarted = false;

unsigned int AMyStateMachProGameModeBase::m_framesToSync = 0;

AMyStateMachProGameModeBase::AMyStateMachProGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	if (NetworkSystem::NetSys != nullptr)
	{
		NetworkSystem::NetSys->setGameMode(this);
		GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("Alive2"));
	}
	player1Name = m_playerName;
	player2Name = m_opponentName;

	MatchCount = static_cast<EMatcheTypes>(m_roundVal);
	transitionMaxDuration = 7.0F;
	slowmotionMaxDuration = 3.5F;
	transitionSpeed = 0.1F;
	roundTime = 60 + (30 * m_timeVal);
	roundTimer = roundTime;
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
	if (UWorld * World = GetWorld())
	{
		MainGameCamera = Cast<AMyCameraActor>(World->SpawnActor(AMyCameraActor::StaticClass(), &FTransform::Identity));
	}


	// Create another player
	if (GetNumPlayers() < 2) // missing online check not sure if necessary
	{

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
	}

	roundTime = 60 + (30 * m_timeVal);
	roundTimer = roundTime;
	player1Name = m_playerName;
	player2Name = m_opponentName;
	enableInputOnRoundStart = true;

	OnName1Changed.Broadcast(player1Name);
	OnName2Changed.Broadcast(player2Name);


	OnMatchNumberChanged.Broadcast(MatchCount);
	OnTimeChanged.Broadcast(roundTimer);

	player1 = Cast<AFGDefaultPawn>(UGameplayStatics::GetPlayerCharacter(this, 0));
	player2 = Cast<AFGDefaultPawn>(UGameplayStatics::GetPlayerCharacter(this, 1));

	player1->Opponent = UGameplayStatics::GetPlayerCharacter(this, 1);
	player2->Opponent = UGameplayStatics::GetPlayerCharacter(this, 0);
	startTimer = prepTime;
	player1Score = 0;
	player2Score = 0;
	roundNumber = 1;

}
void AMyStateMachProGameModeBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (!NetworkSystem::NetSys) {
		++AMyStateMachProGameModeBase::sFrameCounter;
	}


	if (startTimer == prepTime - 3 ||  startTimer == prepTime) {

		player1->AddMovementInput(player1->GetActorForwardVector(), 0.0F);
		player2->AddMovementInput(player2->GetActorForwardVector(), 0.0F);
		player1->doJump = false;
		player2->doJump = false;

		player1->GetMesh()->GetAnimInstance()->StopAllMontages(0.25F);
		player2->GetMesh()->GetAnimInstance()->StopAllMontages(0.25F);
		player1->prepareJump = false;
		player2->prepareJump = false;
		player1->isStunned = false;
		player2->isStunned = false;
		player1->isCrouching = false;
		player2->isCrouching = false;

		if (NetworkSystem::NetSys == nullptr || NetworkSystem::NetSys->roomOwner)
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
		roundTimer = roundTime;
		player1->SetDirectionInputX(0.0F);
		player2->SetDirectionInputX(0.0F);
		player1->movingForward = 0;
		player2->movingForward = 0;

		if (playStartAnimMontage)
		{
			PlayStartUpMontage();
			playStartAnimMontage = false;
		}

	}

	if (!NetworkSystem::startGame && NetworkSystem::NetSys)
	{
		if (!NetworkSystem::firstMessage && NetworkSystem::roomFull)
		{
			NetworkSystem::NetSys->GameMessage(player1->SendInputStream);
			NetworkSystem::firstMessage = false;
			OnName1Changed.Broadcast(player1Name);
			player2Name = m_opponentName;
			OnName2Changed.Broadcast(player2Name);
		}
		else {
			return;
		}
	}
	else if(NetworkSystem::startGame && NetworkSystem::NetSys && !NetworkSystem::firstMessage)
	{
		NetworkSystem::NetSys->GameMessage(player1->SendInputStream);
		NetworkSystem::firstMessage = false;
	}


	if (startTimer < 3.0F && playStartAnim)
	{
		if (!AMyStateMachProGameModeBase::hasGameStarted && NetworkSystem::startGame && NetworkSystem::NetSys)
		{
			AMyStateMachProGameModeBase::hasGameStarted = true;
			OnGameStarted.Broadcast(hasGameStarted);
		}
		else if (!NetworkSystem::NetSys)
		{
			AMyStateMachProGameModeBase::hasGameStarted = true;
			OnGameStarted.Broadcast(hasGameStarted);
		}

		playStartAnim = false;
	}

	if (startTimer > 0.0f )
	{
		if (!NetworkSystem::startGame)
			return;

		player1->isInputEnabled = false;
		player2->isInputEnabled = false;
		startTimer -= DeltaSeconds;
		return;
	}
	if (enableInputOnRoundStart) {
		player1->isInputEnabled = true;
		player2->isInputEnabled = true;
		enableInputOnRoundStart = false;
	}


	//roundTimer -= DeltaSeconds;
	CheckOnWhichSidePlayerIs();

	if (player1->RessourceComp->Health <= 0.0F)
	{
		if(NetworkSystem::NetSys)
		{
		NetworkSystem::NetSys->ticking = false;
		AMyStateMachProGameModeBase::sFrameCounter = 0;
		AMyStateMachProGameModeBase::m_framesToSync = 0;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		}


		//player1->K2_DestroyActor();
		//UGameplayStatics::SetGamePaused(this, true);
		if (!scoreFlag)
		{
			player2Score++;
			player1->playerLost = true;
			//player2->playerWon = true;
			scoreFlag = true;
		}
		OnP2ScoreChanged.Broadcast(player2Score);
		player1->isInputEnabled = false;
		player2->isInputEnabled = false;

		player1->movingForward = 0;
		player2->movingForward = 0;
		player1->isCrouching = false;
		player2->isCrouching = false;
		while (transitiontime < transitionMaxDuration) {
			transitiontime += DeltaSeconds;
			//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Yellow, TEXT("WHILE-TRANSITION"));
			if (transitiontime < slowmotionMaxDuration)
			{
				player1->CustomTimeDilation = transitionSpeed;
				player2->CustomTimeDilation = transitionSpeed;
			}
			else
			{
				player1->CustomTimeDilation = 1.0F;
				player2->CustomTimeDilation = 1.0F;
				player2->playerWon = true;
				CheckIfMatchIsOver(player2Score);

			}
			if (transitionMaxDuration - transitiontime < FadeoutStartTime && playFadeoutFlag && !isMatchOver)
			{
				OnFadeOut.Broadcast(playFadeoutFlag);
				playFadeoutFlag = false;
			}
			return;
		}

		if (!isMatchOver)
		{
			SetupMatch();
			transitiontime = 0;
		}
		else
		{
			OnMatchIsOverCheckIfOnline.Broadcast((bool)NetworkSystem::NetSys, player1->playerWon);

			player1->CustomTimeDilation = 1.0F;
			player2->CustomTimeDilation = 1.0F;
			while (roundTransitionMaxDuration < transitionMaxDuration - 2.0F) {
				roundTransitionMaxDuration += DeltaSeconds;
				//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Yellow, FString::SanitizeFloat(roundTransitionMaxDuration));
				return;
			}
			DetermineMatchWinner();
		}
		//SetupMatch();
		//DetermineMatchWinner();
		//transitiontime = 0;
	}
	if (player2->RessourceComp->Health <= 0.0F)
	{
		if (NetworkSystem::NetSys)
		{
			NetworkSystem::NetSys->ticking = false;
			AMyStateMachProGameModeBase::sFrameCounter = 0;
			AMyStateMachProGameModeBase::m_framesToSync = 0;
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		}
		//player2->K2_DestroyActor();
		//GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("Player1Wins"));
		//UGameplayStatics::SetGamePaused(this, true);
		if (!scoreFlag)
		{
			player1Score++;
			player2->playerLost = true;

			//player1->playerWon = true;
			scoreFlag = true;
		}
		OnP1ScoreChanged.Broadcast(player1Score);
		player1->isInputEnabled = false;
		player2->isInputEnabled = false;
		player1->movingForward = 0;
		player2->movingForward = 0;
		player1->isCrouching = false;
		player2->isCrouching = false;

		while (transitiontime < transitionMaxDuration) {
			transitiontime += DeltaSeconds;
			if (transitiontime < slowmotionMaxDuration)
			{
				player1->CustomTimeDilation = transitionSpeed;
				player2->CustomTimeDilation = transitionSpeed;
			}
			else
			{
				player1->CustomTimeDilation = 1.0F;
				player2->CustomTimeDilation = 1.0F;
				player1->playerWon = true;
				CheckIfMatchIsOver(player1Score);

			}
			if (transitionMaxDuration - transitiontime < FadeoutStartTime && playFadeoutFlag && !isMatchOver)
			{
				OnFadeOut.Broadcast(playFadeoutFlag);
				playFadeoutFlag = false;
			}
			return;
		}

		if (!isMatchOver)
		{
			SetupMatch();
			transitiontime = 0;
		}
		else
		{
			OnMatchIsOverCheckIfOnline.Broadcast((bool)NetworkSystem::NetSys, player1->playerWon);
			player1->CustomTimeDilation = 1.0F;
			player2->CustomTimeDilation = 1.0F;
			while (roundTransitionMaxDuration < transitionMaxDuration - 2.0F) {
				roundTransitionMaxDuration += DeltaSeconds;
				//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Yellow, FString::SanitizeFloat(roundTransitionMaxDuration));
				return;
			}
			DetermineMatchWinner();
		}

	}



	if (roundTimer <= 0.0F)
	{
		if (NetworkSystem::NetSys)
		{
			NetworkSystem::NetSys->ticking = false;
			AMyStateMachProGameModeBase::sFrameCounter = 0;
			AMyStateMachProGameModeBase::m_framesToSync = 0;
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		}
		RoundTimeOver(DeltaSeconds);
		player1->isInputEnabled = false;
		player2->isInputEnabled = false;
		return;
	}

	SetRoundTimer(DeltaSeconds);
}

void AMyStateMachProGameModeBase::SetupMatch()
{
	scoreFlag = false;
	playFadeoutFlag = true;
	startTimer = prepTime - 3;
	roundTimer = roundTime;
	player1->CustomTimeDilation = 1.0F;
	player2->CustomTimeDilation = 1.0F;
	player1->playerWon = false;
	player2->playerWon = false;
	player1->playerLost = false;
	player2->playerLost = false;

	player1->RessourceComp->SetHealth(1.0F);
	player2->RessourceComp->SetHealth(1.0F);
	player1->RessourceComp->SetStunMeter(0.0F);
	player2->RessourceComp->SetStunMeter(0.0F);
	player1->RessourceComp->SetPowerMeter(0.0F);
	player2->RessourceComp->SetPowerMeter(0.0F);
	ResetVictoryMontage();
	ResetLoosingMontage();
	enableInputOnRoundStart = true;
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
	OnNextRoundNumber.Broadcast(roundNumber);
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
			UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");

		}
		if (player2Score == 1)
		{
			UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
			//player 2 wins Complete Match
		}
		break;
	case EMatcheTypes::BestofThree:
		if (player1Score == 2)
		{
			//player 1 wins Complete Match
			UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");

		}
		if (player2Score == 2)
		{
			UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
			//player 2 wins Complete Match
		}
		break;
	case EMatcheTypes::BestofFive:
		if (player1Score == 3)
		{
			UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
			//player 1 wins Complete Match
		}
		if (player2Score == 3)
		{
			UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
			//player 2 wins Complete Match
		}
		break;
	default:
		break;
	}
}

void AMyStateMachProGameModeBase::CheckIfMatchIsOver(int playerScore)
{
	if (NetworkSystem::NetSys == nullptr)
	{
		switch (MatchCount)
		{
		case EMatcheTypes::BestofOne:
			isMatchOver = playerScore > 0 ? true : false;
			if (isMatchOver)
			{
				//OnMatchIsOverCheckIfOnline.Broadcast(false, player1->playerWon);
			}
			break;
		case EMatcheTypes::BestofThree:
			isMatchOver = playerScore > 1 ? true : false;
			if (isMatchOver)
			{
				//OnMatchIsOverCheckIfOnline.Broadcast(false, player1->playerWon);
			}
			break;
		case EMatcheTypes::BestofFive:
			isMatchOver = playerScore > 2 ? true : false;
			if (isMatchOver)
			{
				//OnMatchIsOverCheckIfOnline.Broadcast(false, player1->playerWon);
			}
			break;
		}
	}
	else
	{
		switch (MatchCount)
		{
		case EMatcheTypes::BestofOne:
			isMatchOver = playerScore > 0 ? true : false;
			if (isMatchOver)
			{
				//OnMatchIsOverCheckIfOnline.Broadcast(true, player1->playerWon);
			}
			break;
		case EMatcheTypes::BestofThree:
			isMatchOver = playerScore > 1 ? true : false;
			if (isMatchOver)
			{
				//OnMatchIsOverCheckIfOnline.Broadcast(true, player1->playerWon);
			}
			break;
		case EMatcheTypes::BestofFive:
			isMatchOver = playerScore > 2 ? true : false;
			if (isMatchOver)
			{
				//OnMatchIsOverCheckIfOnline.Broadcast(true, player1->playerWon);
			}
			break;
		}
	}

}

void AMyStateMachProGameModeBase::SetRoundTimer(float deltaSeconds)
{
	if (!NetworkSystem::NetSys->ticking)
	{
		NetworkSystem::NetSys->ticking = true;
	}

	roundTimer -= deltaSeconds;
	OnTimeChanged.Broadcast(this->roundTimer);
}

bool AMyStateMachProGameModeBase::GetRoomOwner()
{
	if(!NetworkSystem::NetSys)
	{
		return true;
	}else
	{
		return NetworkSystem::NetSys->roomOwner;
	}
}

void AMyStateMachProGameModeBase::RoundTimeOver(float DeltaSeconds)
{
	if (player1->RessourceComp->Health > player2->RessourceComp->Health)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0F, FColor::Yellow, TEXT("Player1Wins"));
		//UGameplayStatics::SetGamePaused(this, true);
		//player1Score++;
		//OnP1ScoreChanged.Broadcast(player1Score);

		if (!scoreFlag)
		{
			player1Score++;
			//player2->playerLost = true;

			scoreFlag = true;
		}
		OnP1ScoreChanged.Broadcast(player1Score);
		player1->isInputEnabled = false;
		player2->isInputEnabled = false;
		player1->movingForward = 0;
		player2->movingForward = 0;
		player1->isCrouching = false;
		player2->isCrouching = false;

		while (transitiontime < slowmotionMaxDuration) {
			transitiontime += DeltaSeconds;
			if (transitiontime < 1.0F)
			{
			}
			else
			{
				player1->CustomTimeDilation = 1.0F;
				player2->CustomTimeDilation = 1.0F;
				player1->playerWon = true;
				CheckIfMatchIsOver(player1Score);

			}
			if (slowmotionMaxDuration - transitiontime < FadeoutStartTime && playFadeoutFlag && !isMatchOver)
			{
				OnFadeOut.Broadcast(playFadeoutFlag);
				playFadeoutFlag = false;
			}
			return;
		}
		if (!isMatchOver)
		{
			SetupMatch();
			transitiontime = 0;
		}
		else
		{
			OnMatchIsOverCheckIfOnline.Broadcast(false, player1->playerWon);
			player1->CustomTimeDilation = 1.0F;
			player2->CustomTimeDilation = 1.0F;
			while (roundTransitionMaxDuration < transitionMaxDuration) {
				roundTransitionMaxDuration += DeltaSeconds;
				//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Yellow, FString::SanitizeFloat(roundTransitionMaxDuration));
				return;
			}
			DetermineMatchWinner();
		}
	}
	else if (player1->RessourceComp->Health < player2->RessourceComp->Health)
	{

		if (!scoreFlag)
		{
			player2Score++;
			//player1->playerLost = true;
			scoreFlag = true;
		}
		OnP2ScoreChanged.Broadcast(player2Score);
		player1->isInputEnabled = false;
		player2->isInputEnabled = false;

		player1->movingForward = 0;
		player2->movingForward = 0;
		player1->isCrouching = false;
		player2->isCrouching = false;
		while (transitiontime < slowmotionMaxDuration) {
			transitiontime += DeltaSeconds;
			//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Yellow, TEXT("WHILE-TRANSITION"));
			if (transitiontime < 1.0F)
			{
			}
			else
			{
				player1->CustomTimeDilation = 1.0F;
				player2->CustomTimeDilation = 1.0F;
				player2->playerWon = true;
				CheckIfMatchIsOver(player2Score);

			}
			if (slowmotionMaxDuration - transitiontime < FadeoutStartTime && playFadeoutFlag && !isMatchOver)
			{
				OnFadeOut.Broadcast(playFadeoutFlag);
				playFadeoutFlag = false;
			}
			return;
		}

		if (!isMatchOver)
		{
			SetupMatch();
			transitiontime = 0;
		}
		else
		{
			OnMatchIsOverCheckIfOnline.Broadcast(false, player1->playerWon);
			player1->CustomTimeDilation = 1.0F;
			player2->CustomTimeDilation = 1.0F;
			while (roundTransitionMaxDuration < transitionMaxDuration) {
				roundTransitionMaxDuration += DeltaSeconds;
				//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Yellow, FString::SanitizeFloat(roundTransitionMaxDuration));
				return;
			}
			DetermineMatchWinner();
		}
	}
	else if (player1->RessourceComp->Health == player2->RessourceComp->Health) {
		//draw
		player1->isInputEnabled = false;
		player2->isInputEnabled = false;

		player1->movingForward = 0;
		player2->movingForward = 0;
		player1->isCrouching = false;
		player2->isCrouching = false;
		while (transitiontime < slowmotionMaxDuration) {
			transitiontime += DeltaSeconds;
			//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Yellow, TEXT("WHILE-TRANSITION"));

			return;
		}
		SetupMatch();
	}

}
