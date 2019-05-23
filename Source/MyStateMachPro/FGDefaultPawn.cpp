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



	// Process input

	// Add one atom for stick direction
	const float DirectionThreshold = 0.5f;
	UFGDirectionalInputAtom* InputDirection = nullptr;
	if (DirectionInput.X < -DirectionThreshold)
	{
		if (DirectionInput.Y < -DirectionThreshold)
		{
			InputDirection = DirectionDownBackAtom;;
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			InputDirection = DirectionBackAtom;
			if (CanMoveInLeftDirection) {
				this->AddMovementInput(this->GetActorForwardVector(), -100.0F);

			}
			//this->SetActorLocation(GetActorLocation() + FVector(DirectionInput.X, 0, 0));
		}
		else
		{
			InputDirection = DirectionUpBackAtom;
		}
	}
	else if (DirectionInput.X < DirectionThreshold)
	{
		if (DirectionInput.Y < -DirectionThreshold)
		{
			InputDirection = DirectionDownAtom;
			UE_LOG(LogTemp, Warning, TEXT("i want to crouch"));
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			InputDirection = DirectionNeutralAtom;
		}
		else
		{
			APlayerController* pController = Cast<APlayerController>(GetController());

			InputDirection = DirectionUpAtom;


			UE_LOG(LogTemp, Warning, TEXT("i want to jump"));
			this->Jump();
			//this->AddMovementInput(this->GetActorUpVector(), 1000.0F);
			//LaunchPawn(FVector(0, 0, 1) * 10000.0f,false,false);
		}
	}
	else
	{
		if (DirectionInput.Y < -DirectionThreshold)
		{
			InputDirection = DirectionDownForwardAtom;
			UE_LOG(LogTemp, Warning, TEXT("i want to crouchForward"));
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			InputDirection = DirectionForwardAtom;
			if (CanMoveInRightDirection) {
				this->AddMovementInput(this->GetActorForwardVector(), 100.0F);
			}
			//this->SetActorLocation(GetActorLocation() + FVector(DirectionInput.X, 0, 0));
		}
		else
		{
			InputDirection = DirectionUpForwardAtom;
		}
	}
	InputStream.Add(InputDirection);

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

	// Prune penis old inputs. This would be better-suited to a ringbuffer than an array, but its not much data
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
	FFGMoveLinkToFollow MoveLinkToFollow = CurrentMove->TryLinks(this, InputStream);
	if (MoveLinkToFollow.SMR.CompletionType == EStateMachineCompletionType::Accepted)
	{
		UE_LOG(LogTemp, Warning, TEXT("Switching to state %s"), *MoveLinkToFollow.Link->Move->MoveName.ToString());
		if (MoveLinkToFollow.Link->bClearInput || MoveLinkToFollow.Link->Move->bClearInputOnEntry || CurrentMove->bClearInputOnExit)
		{
			InputStream.Reset();
			InputTimeStamps.Reset();
		}
		else if (MoveLinkToFollow.SMR.DataIndex)
		{
			try {

			// Consume the input we used to get to this move.
			check((MoveLinkToFollow.SMR.DataIndex % (1 + (int32)EFGInputButtons::Count)) == 0);
			InputTimeStamps.RemoveAt(0, MoveLinkToFollow.SMR.DataIndex / 3, false);
			InputStream.RemoveAt(0, MoveLinkToFollow.SMR.DataIndex, false);
			}
			catch (...) {
			}
		}

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


void AFGDefaultPawn::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == Opponent) {
		auto* pAsPawn{ Cast<AFGDefaultPawn>(Opponent) };

		if(OtherComp->GetCollisionProfileName() == pAsPawn->GetCapsuleComponent()->GetCollisionProfileName())
		{
			UE_LOG(LogTemp, Warning, TEXT("Collision is Happening"));
			pAsPawn->RessourceComp->Health -= 25;
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

void AFGDefaultPawn::ReadXAxis(float Value)
{
	// Don't care about clamping. We just need to know negative, zero, or positive.
	DirectionInput.X = Value;
}
void AFGDefaultPawn::ReadYAxis(float Value)
{
	// Don't care about clamping. We just need to know negative, zero, or positive.
	DirectionInput.Y = Value;
}

void AFGDefaultPawn::LeftButtonPressed()
{
	ButtonsDown |= (1 << (int32)EFGInputButtons::LeftFace);
}

void AFGDefaultPawn::LeftButtonReleased()
{
	ButtonsDown &= ~(1 << (int32)EFGInputButtons::LeftFace);
}

void AFGDefaultPawn::TopButtonPressed()
{
	ButtonsDown |= (1 << (int32)EFGInputButtons::TopFace);
}

void AFGDefaultPawn::TopButtonReleased()
{
	ButtonsDown &= ~(1 << (int32)EFGInputButtons::TopFace);
}

void AFGDefaultPawn::RightButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("RightButtonPressed"));
	ButtonsDown |= (1 << (int32)EFGInputButtons::RightFace);
}

void AFGDefaultPawn::RightButtonReleased()
{
	ButtonsDown &= ~(1 << (int32)EFGInputButtons::RightFace);
}
void AFGDefaultPawn::BottomButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("BottomButtonPressed"));
	ButtonsDown |= (1 << (int32)EFGInputButtons::BottomFace);
}

void AFGDefaultPawn::BottomButtonReleased()
{
	ButtonsDown &= ~(1 << (int32)EFGInputButtons::BottomFace);
}


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
