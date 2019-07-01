// Fill out your copyright notice in the Description page of Project Settings.

#include "FGDefaultPawn.h"
#include "MyStateMachPro.h"
#include "FGAtoms.h"
#include "FGMove.h"
#include "FGMoveLink.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Public/MyCameraActor.h"
#include "Engine/World.h"
#include "MyStateMachProGameModeBase.h"


AFGDefaultPawn::AFGDefaultPawn()
{
	// This is ridiculously long, but we ll use it to make a point.
	InputExpirationTime = 0.75f;
	stunTimer = 0.0F;

	//MovementComponent = CreateDefaultSubobject<UPawnMovementComponent>(ADefaultPawn::MovementComponentName);
	//MovementComponent->UpdatedComponent = GetCollisionComponent();
	RessourceComp = CreateDefaultSubobject<URessourceComponent>(TEXT("RComp"));//NewObject<UActorComponent>(this, "RessourceComp");
	MovementRestrictionComp = CreateDefaultSubobject<UMovementRestrictionComponent>(TEXT("MovementRestrictionComp"));//NewObject<UActorComponent>(this, "RessourceComp");
	PlayerRotationComp = CreateDefaultSubobject<UActorRotationComponent>(TEXT("PlayerRotationComp"));//NewObject<UActorComponent>(this, "RessourceComp");

	PunchL = CreateDefaultSubobject<UBoxComponent>(TEXT("PunchL"), true);
	PunchR = CreateDefaultSubobject<UBoxComponent>(TEXT("PunchR"), true);
	KickL = CreateDefaultSubobject<UBoxComponent>(TEXT("KickL"), true);
	KickR = CreateDefaultSubobject<UBoxComponent>(TEXT("KickR"), true);


	PunchL->SetupAttachment(this->GetMesh(), TEXT("HandLSocket"));
	PunchR->SetupAttachment(this->GetMesh(), TEXT("HandRSocket"));
	KickL->SetupAttachment(this->GetMesh(), TEXT("FootLSocket"));
	KickR->SetupAttachment(this->GetMesh(), TEXT("FootRSocket"));

	//OnActorBeginOverlap.AddDynamic(this, &AFGDefaultPawn::OnOverlap);
	PunchL->SetRelativeScale3D(FVector(0.5F, 0.5F, 0.5F));
	PunchR->SetRelativeScale3D(FVector(0.5F, 0.5F, 0.5F));
	KickL->SetRelativeScale3D(FVector(0.5F, 0.5F, 0.5F));
	KickR->SetRelativeScale3D(FVector(0.5F, 0.5F, 0.5F));
}

void AFGDefaultPawn::BeginPlay()
{
	Super::BeginPlay();
	AMyStateMachProGameModeBase* GM = Cast<AMyStateMachProGameModeBase>(UGameplayStatics::GetGameMode(this));


	CanMoveInLeftDirection = true;
	CanMoveInRightDirection = true;
	PunchR->OnComponentBeginOverlap.AddDynamic(this, &AFGDefaultPawn::OnOverlap);
	PunchL->OnComponentBeginOverlap.AddDynamic(this, &AFGDefaultPawn::OnOverlap);
	KickL->OnComponentBeginOverlap.AddDynamic(this, &AFGDefaultPawn::OnOverlap);
	KickR->OnComponentBeginOverlap.AddDynamic(this, &AFGDefaultPawn::OnOverlap);

	this->GetAllChildActors(ColliderParentsArray, false);
	for (AActor* Element : ColliderParentsArray)
	{
		Element->GetName();
		//MoveColliderParents[CurrentMove](TEXT("NALP");
	}
	if(CurrentMove->GetName() == "NA_LP")
	{
		
	}


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

	this->SetRotationOfPlayer();

	HandleStun(DeltaSeconds); // player got stunned 
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
					InputDirection = DirectionDownBackAtom;; // Crouch + Back is On LeftSide
				}
			}
			else
			{
				InputDirection = DirectionDownForwardAtom;; // Crouch + Forward is On RightSide
			}
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			if (this->isOnLeftSide)
			{
				if(bCanBlock)
				{
					bIsBlocking = true;
				}
				else
				{
					InputDirection = DirectionBackAtom; // Back on Leftside
				}
			}
			else
			{
				InputDirection = DirectionForwardAtom; // Forward on Rightside
			}

			if (CanMoveInLeftDirection && !bIsBlocking) {
				this->AddMovementInput(this->GetActorForwardVector(), -100.0F);
			}
		}
		else
		{
			InputDirection = DirectionUpBackAtom; // Jump + Back
			if (this->GetMovementComponent()->IsMovingOnGround())
			{
				//this->GetMovementComponent()->Velocity = (FVector(-600.0F, 0.0F, 600.0F));
				doJump = true;
			}
		}
	} // Neutral Movement
	else if (DirectionInput.X < DirectionThreshold)
	{
		if (DirectionInput.Y < -DirectionThreshold)
		{
			InputDirection = DirectionDownAtom; // Crouch
			isCrouching = true;
			UE_LOG(LogTemp, Warning, TEXT("i want to crouch"));
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			InputDirection = DirectionNeutralAtom; // Idle
			
			isCrouching = false;
		}
		else
		{

			InputDirection = DirectionUpAtom; // Jump
			UE_LOG(LogTemp, Warning, TEXT("i want to jump"));
			//this->Jump();
			if (this->GetMovementComponent()->IsMovingOnGround())
			{
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
				UE_LOG(LogTemp, Warning, TEXT("i want to crouchForward"));
			}else
			{
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
			}
			else
			{
				if(bCanBlock)
				{
					bIsBlocking = true;
				}else
				{
					InputDirection = DirectionBackAtom; // Back on RightSide
				}

			}

			if (CanMoveInRightDirection && !bIsBlocking) {
				this->AddMovementInput(this->GetActorForwardVector(), 100.0F);
			}
		}
		else
		{
			InputDirection = DirectionUpForwardAtom; // Jump Forward
			if (this->GetMovementComponent()->IsMovingOnGround())
			{
				//this->GetMovementComponent()->Velocity = (FVector(600.0F, 0.0F, 600.0F));
				doJump = true;
			}
		}
	}
		this->CrouchValues(isCrouching);
	if(!bCanBlock)
	{
		bIsBlocking = false;
	}
		InputStream.Add(InputDirection);

	if(doJump)
	{
		DiagonalJump(DirectionInput.X, this->GetActorLocation(), DeltaSeconds, jumpHeight, jumpDistance);
	}
	//if (this == Cast<AFGDefaultPawn>(UGameplayStatics::GetPlayerCharacter(this, 0))){	}
	//else{	//InputStream = RecievedInputStream(10);}

	// Add one atom for each buttons state.
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
				UE_LOG(LogTemp, Warning, TEXT("InputStream[0]: %i"), InputTimeStamps[0]);
				InputTimeStamps.RemoveAt(0, i, false);
				InputStream.RemoveAt(0, i * ((int32)EFGInputButtons::Count + 1), false);
			}
			break;
		}
	}
	FFGMoveLinkToFollow MoveLinkToFollow = CurrentMove->TryLinks(this, InputStream);
	if (MoveLinkToFollow.SMR.CompletionType == EStateMachineCompletionType::Accepted && GetCharacterMovement()->IsMovingOnGround())
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
			InputTimeStamps.RemoveAt(0, MoveLinkToFollow.SMR.DataIndex / 3, false);
			InputStream.RemoveAt(0, MoveLinkToFollow.SMR.DataIndex, false);
			//}
			//catch (...) {
			//}
		}

		//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Red, TEXT("In A Move U Can Deal Damage"));

		// Set and start the new move.
		CurrentMove = MoveLinkToFollow.Link->Move;
		TimeInCurrentMove = 0.0f;
		DoMove(CurrentMove);
	}
	else
	{
		TimeInCurrentMove += DeltaSeconds;		// Modulate by move animation length
	}
}


void AFGDefaultPawn::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor == Opponent) {
		auto* pAsPawn{ Cast<AFGDefaultPawn>(Opponent) };
		
		if (OtherComp->GetCollisionProfileName() == pAsPawn->GetCapsuleComponent()->GetCollisionProfileName())
		{
			if (this->canApplyDamage)
			{
				UE_LOG(LogTemp, Warning, TEXT("Collision is Happening"));

				//pAsPawn->gotHit = true;
				//pAsPawn->RessourceComp->ReduceHealth(CurrentMove->DamageValue);
				//pAsPawn->RessourceComp->IncreaseStunMeter(0.05F);
				////pAsPawn->gotHit = false;
				//FVector EmitterSpawnLocation2 = OverlappedComponent->GetComponentLocation();
				//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), gotHitFire, FVector(EmitterSpawnLocation2.X, 0, EmitterSpawnLocation2.Z), FRotator(0.0f, 0.0f, 0.0f), FVector(0.3F, 0.3F, 0.3F), true);

			}
		}
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
		}
		else
		{
			this->GetMesh()->SetRelativeScale3D(FVector(1.0F, 1.0F, 1.0F));
		}
	}
}

void AFGDefaultPawn::SetupPlayerInputComponent(UInputComponent * InInputComponent)
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

void AFGDefaultPawn::ReadXAxis(float Value)
{
	// Don't care about clamping. We just need to know negative, zero, or positive.
	DirectionInput.X = Value;

	if(Value > 0.5)
	{
		SendInputStream.set(11);
		SendInputStream.reset(10);
	}
	else if (Value < -0.5)
	{
		SendInputStream.set(10);
		SendInputStream.reset(11);
	}else
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

/*
 *
void AFGDefaultPawn::ReadInputstream(unsigned short p_keyInput)
{
	std::bitset<12> inputArray(p_keyInput);
	if (inputArray[0])
	{
		this->LeftButtonPressed();
	}
	else {
		this->LeftButtonReleased();
	}
	if (inputArray[1])
	{
		this->TopButtonPressed();
	}
	else {
		this->TopButtonReleased();
	}
	if (inputArray[2])
	{
		this->RightButtonPressed();
	}
	else {
		this->RightButtonReleased();
	}
	if (inputArray[3])
	{
		this->BottomButtonPressed();
	}
	else {
		this->BottomButtonReleased();
	}
	if (inputArray[4])
	{
		this->BumperLeftPressed();
	}
	else {
		this->BumperLeftReleased();
	}
	if (inputArray[5])
	{
		this->TriggerLeftPressed();
	}
	else {
		this->TriggerLeftReleased();
	}
	if (inputArray[6])
	{
		this->BumperRightPressed()
	}
	else {
		this->BumperRightReleased()
	}
	if (inputArray[7])
	{
		this->TriggerRightPressed()
	}
	else {
		this->TriggerRightReleased()
	}
	if (inputArray[8])
	{
		this->ReadYAxis(1)
	}
	else {
		this->ReadYAxis(0)
	}
	if (inputArray[9])
	{
		this->ReadYAxis(-1)
	}
	else {
		this->ReadYAxis(0)
	}
	if (inputArray[10])
	{
		this->ReadXAxis(1)
	}
	else {
		this->ReadXAxis(0)
	}
	if (inputArray[11])
	{
		this->ReadXAxis(-1)
	}
	else {
		this->ReadXAxis(0)
	}
}
 */

}


void AFGDefaultPawn::DiagonalJump(float direction, FVector position, float time, float Height, float Distance)
{

	if (gotHit) // jump direction
	{
		jumpInitializeFlag = false;
		doJump = false;
		return;
	}

	if(!jumpInitializeFlag) // change to while ? 
	{
		timeInJump = 0;
		jumpStartLocation = FVector(this->GetActorLocation().X, this->GetActorLocation().Y, this->GetActorLocation().Z);
		directionmodifier = FMath::Sign(direction);

		jumpInitializeFlag = true;
	}
		if (timeInJump <= jumpDuration)
		{
			timeInJump += time;
			GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Yellow, FString::SanitizeFloat(timeInJump/jumpDuration));
			float curveValue = DiagonalCurve->GetFloatValue(timeInJump/jumpDuration);

			jumpTargetLocation.X = FMath::Lerp(jumpStartLocation.X, jumpStartLocation.X + (jumpDistance * directionmodifier), timeInJump / jumpDuration);
			this->SetActorLocation(FVector(jumpTargetLocation.X, jumpStartLocation.Y, jumpStartLocation.Z + (jumpHeight * curveValue)));
		}else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Red, TEXT("Reset"));

			jumpInitializeFlag = false;
			doJump = false;

		}
	//timeInJump += time;



}

void AFGDefaultPawn::HandleStun(float deltaSeconds)
{
	if (isStunned || gotHit)
	{
		DisableInput(Cast<APlayerController>(this));
		stunTimer += deltaSeconds;
		if (stunTimer >= 2.0F)
		{
			gotHit = false;
			stunTimer = 0.0F;
		}
		return;
	}
	else
	{
		EnableInput(Cast<APlayerController>(this));
	}
}

void AFGDefaultPawn::CrouchValues(bool inCrouch)
{
	if(inCrouch)
	{
		this->GetCapsuleComponent()->SetCapsuleRadius(34.0F, true);
		this->Crouch();
	}else
	{
		this->UnCrouch();
		GetCapsuleComponent()->SetCapsuleRadius(40.0F,true);
	}
}

void AFGDefaultPawn::MoveColliderSwitch()
{
	//MoveColliderParents[CurrentMove]->SetActive(true);

}

