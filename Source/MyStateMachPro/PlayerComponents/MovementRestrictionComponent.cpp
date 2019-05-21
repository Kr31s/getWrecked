// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementRestrictionComponent.h"
#include "GameFramework/Actor.h"
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
	MaxDistanceFromMiddle = 500.0F;
	YValueForPlayer = 0.0F;

	//Self = GetOwner();
	//Enemy = UGameplayStatics::GetPlayerCharacter(this, 1);
	//Enemy = UGameplayStatics::GetPlayerController(GetWorld(), 1);//->GetOwner();
	//FString p1Name = Self->GetName();
	//FString p2Name = Enemy->GetName();

	
	////if(UGameplayStatics::GetPlayerControllerID(PlayerOne->) == 1)
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *p1Name);
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *p2Name);

	// ...
	// 860 CameraViewUnits, 430 per side from middlepoint
}


// Called every frame
void UMovementRestrictionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Self && Enemy)
	{
		FVector P1L = Self->GetActorLocation();
		FVector P2L = Enemy->GetActorLocation();
		float playerDistance = Self->GetDistanceTo(Enemy);
		GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Green, FString::SanitizeFloat(playerDistance));

	if (playerDistance >= MaxDistanceFromMiddle && P1L.X < P2L.X) {
		float LeftEdge = ((P1L.X + P2L.X) * 0.5F) - ((MaxDistanceFromMiddle )/2);
		//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Black, TEXT("MAXIMUMPlayerDistance"));
		//PlayerOne->SetActorLocation(FVector(FMath::Clamp(P1L.X, LeftCornerYValue, RightCornerYValue), 0.0F, P1L.Z));
		Self->SetActorLocation(FVector(LeftEdge, P1L.Y, P1L.Z));
	}
	else if (playerDistance >= MaxDistanceFromMiddle && P1L.X > P2L.X) {
		float RightEdge = ((P1L.X + P2L.X) * 0.5F) + ((MaxDistanceFromMiddle) / 2);
		GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Red, TEXT("MAXIMUMPlayerDistance"));
		//PlayerOne->SetActorLocation(FVector(FMath::Clamp(P1L.X, LeftCornerYValue, RightCornerYValue), 0.0F, P1L.Z));
		Self->SetActorLocation(FVector(RightEdge, P1L.Y, P1L.Z));
	}
	}
	/*
	else {

	}
	*/
	
	// ...
}

