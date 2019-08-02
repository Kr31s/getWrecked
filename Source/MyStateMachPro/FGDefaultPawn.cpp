// Fill out your copyright notice in the Description page of Project Settings.

#include "FGDefaultPawn.h"
#include "MyStateMachPro.h"
#include "FGAtoms.h"
#include "FGMove.h"
#include "FGMoveLink.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Public/MyCameraActor.h"
#include "MyStateMachProGameModeBase.h"
#include "Lobby/NetworkSystem.h"


void AFGDefaultPawn::DoMovesFromInputStream(std::bitset<12> inputStream)
{
	(inputStream[0]) ? this->LeftButtonPressed() : this->LeftButtonReleased();
	(inputStream[1]) ? this->TopButtonPressed() : this->TopButtonReleased();
	(inputStream[2]) ? this->RightButtonPressed() : this->RightButtonReleased();
	(inputStream[3]) ? this->BottomButtonPressed() : this->BottomButtonReleased();
	(inputStream[4]) ? this->BumperLeftPressed() : this->BumperLeftReleased();
	(inputStream[5]) ? this->TriggerLeftPressed() : this->TriggerLeftReleased();
	(inputStream[6]) ? this->BumperRightPressed() : this->BumperRightReleased();
	(inputStream[7]) ? this->TriggerRightPressed() : this->TriggerRightReleased();
	(inputStream[8]) ? this->ReadYAxis(1) : (inputStream[9]) ? this->ReadYAxis(-1) : this->ReadYAxis(0);
	(inputStream[10]) ? this->ReadXAxis(-1) : (inputStream[11]) ? this->ReadXAxis(1) : this->ReadXAxis(0);
}

AFGDefaultPawn::AFGDefaultPawn()
{
	// This is ridiculously long, but we ll use it to make a point.
	stunTimer = 0.0F;

	RessourceComp = CreateDefaultSubobject<URessourceComponent>(TEXT("RComp"));//NewObject<UActorComponent>(this, "RessourceComp");
	MovementRestrictionComp = CreateDefaultSubobject<UMovementRestrictionComponent>(TEXT("MovementRestrictionComp"));//NewObject<UActorComponent>(this, "RessourceComp");
	PlayerRotationComp = CreateDefaultSubobject<UActorRotationComponent>(TEXT("PlayerRotationComp"));//NewObject<UActorComponent>(this, "RessourceComp");

}

void AFGDefaultPawn::BeginPlay()
{
	Super::BeginPlay();
	AMyStateMachProGameModeBase* GM = Cast<AMyStateMachProGameModeBase>(UGameplayStatics::GetGameMode(this));

	CanMoveInLeftDirection = true;
	CanMoveInRightDirection = true;

	this->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AFGDefaultPawn::OnHit);
	//this->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AFGDefaultPawn::ExitOverlap);
	if (!CurrentMove) {
		UE_LOG(LogTemp, Warning, TEXT("No initial move."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Linked Move: %s"), *CurrentMove->LinkedMoves[0]->Move->MoveName.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Linked Move: "), *CurrentMove->MoveName.ToString());

	}
	// Check that we have all the states we need.
	if (!DirectionDownBackAtom || !DirectionDownAtom || !DirectionDownForwardAtom
		|| !DirectionBackAtom || !DirectionNeutralAtom || !DirectionForwardAtom
		|| !DirectionUpBackAtom || !DirectionUpAtom || !DirectionUpForwardAtom)
	{

		UE_LOG(LogTemp, Warning, TEXT("Missing at least one directinal atom."));
		Destroy();
		return;
	}
	int ButtonInputCount = (int32)EFGButtonState::Count;
	for (int32 i = 0; i < ButtonInputCount; ++i)
	{
		if (!ButtonAtoms.IsValidIndex(i) || !ButtonAtoms[i])
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough button input atoms, or a NULL entry was found in the "));
			Destroy();
			return;
		}
	}
	GetWorldTimerManager().SetTimerForNextTick(this, &AFGDefaultPawn::UseGameCamera);
}

void AFGDefaultPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	GetCharacterMovement()->Velocity = FVector(GetVelocity().X, 0.0F, -700.0F);
	this->SetRotationOfPlayer();

	if (isStunned)
	{
		HandleStun(DeltaSeconds); // player got stunned
		if (NetworkSystem::NetSys && AMyStateMachProGameModeBase::hasGameStarted
			&& UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(GetController())) == 0) {
			FrameSyncCheck();
			NetworkSystem::NetSys->GameMessage(SendInputStream);
		}
		return;
	}
	EnablePlayerInput(isInputEnabled);

	if (!isInputEnabled) {
		if (NetworkSystem::NetSys && AMyStateMachProGameModeBase::hasGameStarted 
			&& UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(GetController())) == 0) {
			FrameSyncCheck();
			NetworkSystem::NetSys->GameMessage(SendInputStream);
		}
		return;
	}


	// Process input

	// Add one atom for stick direction
	const float DirectionThreshold = 0.5f;
	UFGDirectionalInputAtom* InputDirection = nullptr;
	//Back Movement
	if (DirectionInput.X < -DirectionThreshold)
	{
		if (DirectionInput.Y < -DirectionThreshold)
		{
			if (this->isOnLeftSide)
			{
				if (bCanBlock)
				{
					bIsBlocking = true;
				}
				else
				{
					isCrouching = true;
					InputDirection = DirectionDownBackAtom;; // Crouch + Back is On LeftSide
				}
			}
			else
			{
				isCrouching = true;

				InputDirection = DirectionDownForwardAtom;; // Crouch + Forward is On RightSide
			}
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			if (this->isOnLeftSide)
			{
				if (bCanBlock)
				{
					bIsBlocking = true;
					//CurrentMove = BlockMove;
					InputDirection = DirectionBackAtom; // Back on Leftside

				}
				else
				{
					InputDirection = DirectionBackAtom; // Back on Leftside
					movingForward = -1;

				}
			}
			else
			{
				InputDirection = DirectionForwardAtom; // Forward on Rightside
				movingForward = 1;

			}

			if (CanMoveInLeftDirection && !bIsBlocking) {
				this->AddMovementInput(this->GetActorForwardVector(), -100.0F);
				isCrouching = false;
				MovementRestrictionComp->TickComponent();

			}
		}
		else
		{
			if (this->isOnLeftSide)
			{
				movingForward = -1;
				InputDirection = DirectionUpBackAtom; // Jump + Back
				if (this->GetMovementComponent()->IsMovingOnGround() && !doJump && CurrentMove == BW_Jump)
				{
					//this->GetMovementComponent()->Velocity = (FVector(-600.0F, 0.0F, 600.0F));
					isCrouching = false;
					movingForward = -1;

					doJump = true;
				}
			}
			else
			{
				movingForward = 1;
				InputDirection = DirectionUpForwardAtom; // Jump Forward
				if (this->GetMovementComponent()->IsMovingOnGround() && !doJump && CurrentMove == FW_Jump)
				{
					//this->GetMovementComponent()->Velocity = (FVector(-600.0F, 0.0F, 600.0F));
					isCrouching = false;
					movingForward = 1;

					doJump = true;
				}

			}

		}
	} // Neutral Movement
	else if (DirectionInput.X < DirectionThreshold)
	{
		if (DirectionInput.Y < -DirectionThreshold)
		{
			InputDirection = DirectionDownAtom; // Crouch
			isCrouching = true;
			//UE_LOG(LogTemp, Warning, TEXT("i want to crouch"));
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			InputDirection = DirectionNeutralAtom; // Idle
			movingForward = 0;
			isCrouching = false;
		}
		else if (DirectionInput.Y > 0.9F)
		{
			isCrouching = false;

			InputDirection = DirectionUpAtom; // Jump
			//UE_LOG(LogTemp, Warning, TEXT("i want to jump"));
			//this->Jump();
			if (this->GetMovementComponent()->IsMovingOnGround() && !doJump && CurrentMove == NeutralJump)
			{
				movingForward = 0;
				//this->GetMovementComponent()->Velocity = (FVector(600.0F, 0.0F, 600.0F));
				doJump = true;
			}
		}
	}
	else// Forward Movement
	{
		if (DirectionInput.Y < -DirectionThreshold)
		{
			if (this->isOnLeftSide)
			{
				InputDirection = DirectionDownForwardAtom; // Crouch + Forward on LeftSide
				isCrouching = true;
			}
			else
			{
				isCrouching = true;

				if (bCanBlock)
				{
					bIsBlocking = true;
				}
				else
				{
					InputDirection = DirectionDownBackAtom; // Crouch + Back on RightSide
				}
			}
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			if (this->isOnLeftSide)
			{
				InputDirection = DirectionForwardAtom; // Forward on LeftSide
				movingForward = 1;
			}
			else
			{
				if (bCanBlock)
				{
					bIsBlocking = true;
					//CurrentMove = BlockMove;
					InputDirection = DirectionBackAtom; // Back on RightSide // Block

				}
				else
				{
					InputDirection = DirectionBackAtom; // Back on RightSide
					movingForward = -1;

				}

			}

			if (CanMoveInRightDirection && !bIsBlocking) {
				isCrouching = false;

				this->AddMovementInput(this->GetActorForwardVector(), 100.0F);
				MovementRestrictionComp->TickComponent();
			}
		}
		else
		{
			if (this->isOnLeftSide)
			{
				isCrouching = false;

				movingForward = 1;
				InputDirection = DirectionUpForwardAtom; // Jump Forward
				if (this->GetMovementComponent()->IsMovingOnGround() && !doJump && CurrentMove == FW_Jump)
				{
					//this->GetMovementComponent()->Velocity = (FVector(600.0F, 0.0F, 600.0F));
					movingForward = 1;

					doJump = true;
				}
			}
			else
			{
				isCrouching = false;

				movingForward = -1;
				InputDirection = DirectionUpBackAtom; // Jump Back
				if (this->GetMovementComponent()->IsMovingOnGround() && !doJump && CurrentMove == BW_Jump)
				{
					//this->GetMovementComponent()->Velocity = (FVector(600.0F, 0.0F, 600.0F));
					movingForward = -1;

					doJump = true;
				}
			}

		}
	}
	this->CrouchValues(isCrouching);
	if (!bCanBlock)
	{
		bIsBlocking = false;
	}
	InputStream.Add(InputDirection);

	if (doJump)
	{
		DiagonalJump(DirectionInput.X, this->GetActorLocation(), DeltaSeconds, jumpHeight, jumpDistance);
		MovementRestrictionComp->TickComponent();
	}
	//if (this == Cast<AFGDefaultPawn>(UGameplayStatics::GetPlayerCharacter(this, 0))){	}
	//else{	//InputStream = RecievedInputStream(10);}

	// Add one atom for each buttons state.
	if (NetworkSystem::NetSys && AMyStateMachProGameModeBase::hasGameStarted
		&& UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(GetController())) == 0) {
	for (int32 i = 0; i < (int32)EFGInputButtons::Count; ++i)
	{
		if (ButtonsDown & (1 << i))
		{
			if (ButtonsDown_Old & (1 << i))
			{
				InputStream.Add(ButtonAtoms[(int32)EFGButtonState::HeldDown]);
			}
			else
			{
				InputStream.Add(ButtonAtoms[(int32)EFGButtonState::JustPressed]);
			}
		}
		else
		{
			InputStream.Add(ButtonAtoms[(int32)EFGButtonState::Up]);
		}
	}
		FrameSyncCheck();
		NetworkSystem::NetSys->GameMessage(SendInputStream);

	// Cache old button state so we can distinguish between held and just pressed.
	ButtonsDown_Old = ButtonsDown;

	// Always add an input time stamp to match the input sequence.
	float CurrentTime = UKismetSystemLibrary::GetGameTimeInSeconds(this);
	InputTimeStamps.Add(CurrentTime);

	// Prune old inputs. This would be better-suited to a ringbuffer than an array, but its not much data
	for (int32 i = 0; i < InputStream.Num(); ++i)
	{
		if ((InputTimeStamps[i] + InputExpirationTime) >= CurrentTime)
		{
			// Remove everything before this, then exit the loop.
			if (i > 0)
			{
				InputTimeStamps.RemoveAt(0, i, false);
				InputStream.RemoveAt(0, i * ((int32)EFGInputButtons::Count + 1), false);
			}
			break;
		}
	}
	}
	else if(NetworkSystem::NetSys && AMyStateMachProGameModeBase::hasGameStarted){
		for (int i = 0; i < 249; ++i)
		{
			if (NetworkSystem::NetSys->gameMessagesRivale[i].m_time == AMyStateMachProGameModeBase::sFrameCounter - 9) {
				DoMovesFromInputStream(std::bitset<12>(NetworkSystem::NetSys->gameMessagesRivale[i].m_input));
				break;
			}
		}
	}
	FFGMoveLinkToFollow MoveLinkToFollow = CurrentMove->TryLinks(this, InputStream);
	if (MoveLinkToFollow.SMR.CompletionType == EStateMachineCompletionType::Accepted/* && GetCharacterMovement()->IsMovingOnGround() -- check if everything works as intended*/)
	{

		UE_LOG(LogTemp, Warning, TEXT("Switching to state %s"), *MoveLinkToFollow.Link->Move->MoveName.ToString());

		if (MoveLinkToFollow.Link->bClearInput || MoveLinkToFollow.Link->Move->bClearInputOnEntry || CurrentMove->bClearInputOnExit)
		{
			InputStream.Reset();
			InputTimeStamps.Reset();
		}
		else if (MoveLinkToFollow.SMR.DataIndex)
		{
			//try {
			GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Orange, TEXT("MOVE"));

			// Consume the input we used to get to this move.

			check((MoveLinkToFollow.SMR.DataIndex % (1 + (int32)EFGInputButtons::Count)) == 0);
			InputTimeStamps.RemoveAt(0, MoveLinkToFollow.SMR.DataIndex / 5, false);
			InputStream.RemoveAt(0, MoveLinkToFollow.SMR.DataIndex, false);
			//}
			//catch (const std::exception e) {
			//}
		}

		//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Red, TEXT("In A Move U Can Deal Damage"));

		// Set and start the new move.
		CurrentMove = MoveLinkToFollow.Link->Move;
		//if (this == UGameplayStatics::GetPlayerCharacter(this, 0))
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("CurrentMove: %s"), *CurrentMove->MoveName.ToString());
		//	UE_LOG(LogTemp, Warning, TEXT("CurrentMove: %s"), *CurrentMove->GetName());
		//}

		TimeInCurrentMove = 0.0f;
		DoMove(CurrentMove);
		this->RessourceComp->IncreasePowerMeter(CurrentMove->PowerMeterRaiseValue / 2);
	}
	else
	{
		TimeInCurrentMove += DeltaSeconds;		// Modulate by move animation length
	}
}

void AFGDefaultPawn::FrameSyncCheck()
{
	if (UGameplayStatics::GetGlobalTimeDilation(GetWorld()) == 2)
	{
		NetworkSystem::NetSys->GameMessage(SendInputStream);
		--AMyStateMachProGameModeBase::m_framesToSync;
		if (AMyStateMachProGameModeBase::m_framesToSync <= 0)
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		}
	}
	else if (AMyStateMachProGameModeBase::m_framesToSync > 0) {
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 2);
	}

}

void AFGDefaultPawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == Opponent) {
		auto* pAsPawn{ Cast<AFGDefaultPawn>(Opponent) };
		bCollisionWithOppenent = true;
		if (doJump && pAsPawn->doJump)
		{
			doJump = false;
			jumpInitializeFlag = false;
		}
	}
}


void AFGDefaultPawn::ExitOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == Opponent) {
		auto* pAsPawn{ Cast<AFGDefaultPawn>(Opponent) };
		bCollisionWithOppenent = false;
	}

}
UFGMove* AFGDefaultPawn::GetCurrentMove()
{
	return CurrentMove;
}

void AFGDefaultPawn::SetRotationOfPlayer()
{
	if (GetMovementComponent()->IsMovingOnGround())
	{
		if (this->isOnLeftSide)
		{
			this->GetMesh()->SetRelativeScale3D(FVector(1.0F, -1.0F, 1.0F));
			//GetMesh()->SkeletalMesh->SkelMirrorAxis = EAxis::X;

		}
		else
		{
			this->GetMesh()->SetRelativeScale3D(FVector(1.0F, 1.0F, 1.0F));
			//GetMesh()->SkeletalMesh->SkelMirrorAxis = EAxis::Y;

		}
	}
}

void AFGDefaultPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	InInputComponent->BindAxis("XAxis", this, &AFGDefaultPawn::ReadXAxis);
	InInputComponent->BindAxis("YAxis", this, &AFGDefaultPawn::ReadYAxis);
	InInputComponent->BindAction("LeftButton", IE_Pressed, this, &AFGDefaultPawn::LeftButtonPressed);
	InInputComponent->BindAction("LeftButton", IE_Released, this, &AFGDefaultPawn::LeftButtonReleased);
	InInputComponent->BindAction("TopButton", IE_Pressed, this, &AFGDefaultPawn::TopButtonPressed);
	InInputComponent->BindAction("TopButton", IE_Released, this, &AFGDefaultPawn::TopButtonReleased);
	InInputComponent->BindAction("RightButton", IE_Pressed, this, &AFGDefaultPawn::RightButtonPressed);
	InInputComponent->BindAction("RightButton", IE_Released, this, &AFGDefaultPawn::RightButtonReleased);
	InInputComponent->BindAction("BottomButton", IE_Pressed, this, &AFGDefaultPawn::BottomButtonPressed);
	InInputComponent->BindAction("BottomButton", IE_Released, this, &AFGDefaultPawn::BottomButtonReleased);
}

void AFGDefaultPawn::checkBlock()
{
	if (bCanBlock && DirectionInput.X < 0 && isOnLeftSide)
	{
		bIsBlocking = true;
		return;
	}
	if (bCanBlock && DirectionInput.X > 0 && !isOnLeftSide)
	{
		bIsBlocking = true;
		return;
	}
}

void AFGDefaultPawn::EnablePlayerInput(bool isEnabled)
{
	if (isEnabled) {
		EnableInput(Cast<APlayerController>(GetController()));
		EnableInput(Cast<APlayerController>(this));
	}
	else {
		DisableInput(Cast<APlayerController>(GetController()));
		DisableInput(Cast<APlayerController>(this));
		return;
	}
}

void AFGDefaultPawn::ReadXAxis(float Value)
{
	// Don't care about clamping. We just need to know negative, zero, or positive.
	DirectionInput.X = Value;

	if (Value > 0.5)
	{
		SendInputStream.set(11);
		SendInputStream.reset(10);
	}
	else if (Value < -0.5)
	{
		SendInputStream.set(10);
		SendInputStream.reset(11);
	}
	else
	{
		SendInputStream.reset(11);
		SendInputStream.reset(10);
	}

}
void AFGDefaultPawn::ReadYAxis(float Value)
{
	// Don't care about clamping. We just need to know negative, zero, or positive.
	DirectionInput.Y = Value;

	if (Value > 0.5)
	{
		SendInputStream.set(8);
		SendInputStream.reset(9);
	}
	else if (Value < -0.5)
	{
		SendInputStream.set(9);
		SendInputStream.reset(8);
	}
	else
	{
		SendInputStream.reset(8);
		SendInputStream.reset(9);
	}
}

void AFGDefaultPawn::LeftButtonPressed()
{
	ButtonsDown |= (1 << (int32)EFGInputButtons::LeftFace);
	SendInputStream.set(0);
}

void AFGDefaultPawn::LeftButtonReleased()
{
	ButtonsDown &= ~(1 << (int32)EFGInputButtons::LeftFace);
	SendInputStream.reset(0);
}

void AFGDefaultPawn::TopButtonPressed()
{
	ButtonsDown |= (1 << (int32)EFGInputButtons::TopFace);
	SendInputStream.set(1);
}

void AFGDefaultPawn::TopButtonReleased()
{
	ButtonsDown &= ~(1 << (int32)EFGInputButtons::TopFace);
	SendInputStream.reset(1);
}

void AFGDefaultPawn::RightButtonPressed()
{
	ButtonsDown |= (1 << (int32)EFGInputButtons::RightFace);
	SendInputStream.set(2);
}

void AFGDefaultPawn::RightButtonReleased()
{
	ButtonsDown &= ~(1 << (int32)EFGInputButtons::RightFace);
	SendInputStream.reset(2);
}
void AFGDefaultPawn::BottomButtonPressed()
{
	ButtonsDown |= (1 << (int32)EFGInputButtons::BottomFace);
	SendInputStream.set(3);
}

void AFGDefaultPawn::BottomButtonReleased()
{
	ButtonsDown &= ~(1 << (int32)EFGInputButtons::BottomFace);
	SendInputStream.reset(3);
}

void AFGDefaultPawn::TriggerLeftPressed()
{
	SendInputStream.set(5);
}
void AFGDefaultPawn::TriggerLeftReleased()
{
	SendInputStream.reset(5);
}
void AFGDefaultPawn::TriggerRightPressed()
{
	SendInputStream.set(7);
}
void AFGDefaultPawn::TriggerRightReleased()
{
	SendInputStream.reset(7);
}
void AFGDefaultPawn::BumperLeftPressed()
{
	SendInputStream.set(4);
}
void AFGDefaultPawn::BumperLeftReleased()
{
	SendInputStream.reset(4);
}
void AFGDefaultPawn::BumperRightPressed()
{
	SendInputStream.set(6);
}
void AFGDefaultPawn::BumperRightReleased()
{
	SendInputStream.reset(6);
}

//TArray<USM_InputAtom*> AFGDefaultPawn::ReadInputstream()
//{
//	InputStream =
//		return InputStream;
//}


void AFGDefaultPawn::UseGameCamera()
{
	if (AMyStateMachProGameModeBase * GM = Cast<AMyStateMachProGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (APlayerController * PC = Cast<APlayerController>(GetController()))
		{
			if (AMyCameraActor * Cam = Cast<AMyCameraActor>(GM->MainGameCamera))
			{
				if (UGameplayStatics::GetPlayerControllerID(PC) == 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("Player %i registering with game camera (one)"), UGameplayStatics::GetPlayerControllerID(PC));
					Cam->PlayerOne = this;
					this->MovementRestrictionComp->Self = this;
					this->MovementRestrictionComp->Enemy = UGameplayStatics::GetPlayerCharacter(this, 1);;

				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Player %i registering with game camera (two)"), UGameplayStatics::GetPlayerControllerID(PC));
					Cam->PlayerTwo = this;
					this->MovementRestrictionComp->Self = this;
					this->MovementRestrictionComp->Enemy = UGameplayStatics::GetPlayerCharacter(this, 0);;
				}
				PC->SetViewTarget(GM->MainGameCamera);
				return;
			}
		}
	}
	// Try again next frame. Currently, there's no limit to how many times we'll do this.
	GetWorldTimerManager().SetTimerForNextTick(this, &AFGDefaultPawn::UseGameCamera);
}


void AFGDefaultPawn::DiagonalJump(float direction, FVector position, float time, float Height, float Distance)
{
	if (gotHit) // jump direction
	{
		jumpInitializeFlag = false;
		doJump = false;
		return;
	}

	if (!jumpInitializeFlag)
	{
		timeInJump = 0;
		jumpStartLocation = FVector(this->GetActorLocation().X, this->GetActorLocation().Y, this->GetActorLocation().Z);
		directionmodifier = FMath::Sign(direction);
		prepareJump = true;
		jumpInitializeFlag = true;
	}
	if (timeInJump <= jumpDuration)
	{
		timeInJump += time;
		float curveValue = DiagonalCurve->GetFloatValue(timeInJump / jumpDuration);

		jumpTargetLocation.X = FMath::Lerp(jumpStartLocation.X, jumpStartLocation.X + (jumpDistance * directionmodifier), timeInJump / jumpDuration);


		if(timeInJump > 0.5F && prepareJump)
		{
			prepareJump = false;
		}

		if(isCrouching)
		{
			GetCapsuleComponent()->SetCapsuleHalfHeight(60.0F);
			this->GetCapsuleComponent()->SetCapsuleRadius(34.0F, true);

		}else
		{
			GetCapsuleComponent()->SetCapsuleHalfHeight(90.0F);
			this->GetCapsuleComponent()->SetCapsuleRadius(40.0F, true);

		}


		if (this->CanMoveInLeftDirection && directionmodifier <= 0
			|| this->CanMoveInRightDirection && directionmodifier >= 0)
		{
			this->SetActorLocation(FVector(jumpTargetLocation.X, 0.0F, jumpStartLocation.Z + (jumpHeight * curveValue)));
		}
		else
		{
			this->SetActorLocation(FVector(this->GetActorLocation().X, 0.0F, jumpStartLocation.Z + (jumpHeight * curveValue)));
		}

		// Push opponent Away do land on destination point
		if ((timeInJump / jumpDuration) > 0.8 && bCollisionWithOppenent)
		{
			//opponent left from me
			if (this->GetActorLocation().X > Opponent->GetActorLocation().X)
			{
				Opponent->SetActorLocation(FVector(Opponent->GetActorLocation().X - 15.0F, Opponent->GetActorLocation().Y, Opponent->GetActorLocation().Z));
			}
			else //opponent right from me
			{
				Opponent->SetActorLocation(FVector(Opponent->GetActorLocation().X + 15.0F, Opponent->GetActorLocation().Y, Opponent->GetActorLocation().Z));
			}
		}
		this->GetMovementComponent()->Velocity = FVector(0.0F, 0.0F, 0.0F);
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Red, TEXT("Reset"));
		GetMesh()->GetAnimInstance()->StopAllMontages(0.0F);
		jumpInitializeFlag = false;
		doJump = false;
		timeInJump = 0;
		GetCapsuleComponent()->SetCapsuleHalfHeight(100.0F);

	}
}
int AFGDefaultPawn::DirectionSign()
{
	return directionmodifier;
}

void AFGDefaultPawn::HandleStun(float deltaSeconds)
{

	//DisableInput(Cast<APlayerController>(this));
	stunTimer += deltaSeconds;
	if (gotHit && stunTimer > 0.5F)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Red, TEXT("GOT HIT Check"));
		stunTimer = 0.0F;
		isStunned = false;
		ResetStunMontage();
	}
	else if (stunTimer >= 5.6F)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Red, TEXT("Reset STUN"));
		//gotHit = false;
		stunTimer = 0.0F;
		isStunned = false;
		ResetStunMontage();

	}


}
void AFGDefaultPawn::CrouchValues(bool inCrouch)
{
	if (inCrouch)
	{
		this->GetCapsuleComponent()->SetCapsuleRadius(34.0F, true);
		this->Crouch();
	}
	else
	{
		this->UnCrouch();
		GetCapsuleComponent()->SetCapsuleRadius(40.0F, true);
	}
}
void AFGDefaultPawn::MoveColliderSwitch()
{
	//MoveColliderParents[CurrentMove]->SetActive(true);

}
