// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementRestrictionComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "FGDefaultPawn.h"
#include "MyStateMachPro.h"

// Sets default values for this component's properties
UMovementRestrictionComponent::UMovementRestrictionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//APlayerController* PC = Cast<APlayerController>(GetController());
	// ...
}


// Called when the game starts
void UMovementRestrictionComponent::BeginPlay()
{
	Super::BeginPlay();
	LeftCornerYValue = -965.0F;
	MaxDistanceFromMiddle = 550.0F;
	YValueForPlayer = 0.0F;

}


// Called every frame
void UMovementRestrictionComponent::TickComponent()
{
	if (Self && Enemy)
	{
		auto* pAsPawn{ Cast<AFGDefaultPawn>(Self) };
		FVector P1L = Self->GetActorLocation();
		float LeftEdge = ((P1L.X + Enemy->GetActorLocation().X) * 0.5F) - ((MaxDistanceFromMiddle) / 2);
		float RightEdge = ((P1L.X + Enemy->GetActorLocation().X) * 0.5F) + ((MaxDistanceFromMiddle) / 2);

		if (P1L.X <= LeftEdge) {
			pAsPawn->CanMoveInLeftDirection = false;
			//Self->SetActorLocation(FVector(LeftEdge, P1L.Y, P1L.Z));
		}
		else
		{
			pAsPawn->CanMoveInLeftDirection = true;

		}
		if (P1L.X >= RightEdge) {
			pAsPawn->CanMoveInRightDirection = false;
			//Self->SetActorLocation(FVector(RightEdge, P1L.Y, P1L.Z));
		}
		else
		{
			pAsPawn->CanMoveInRightDirection = true;

		}
	}
}

void UMovementRestrictionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	if (Self && Enemy)
	{
		Self->SetActorLocation(FVector(FMath::Clamp(Self->GetActorLocation().X, LeftMapEnd, RightMapEnd), 0.0F, Self->GetActorLocation().Z));
		TickComponent();
	}
}

