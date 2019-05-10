// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCameraActor.h"
#include "MyStateMachPro.h"

AMyCameraActor::AMyCameraActor()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring"));
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	SpringArm->SetupAttachment(RootComponent);
	GetCameraComponent()->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	VerticalOffset = 150.0f;

	PrimaryActorTick.bCanEverTick = true;
}

void AMyCameraActor::Tick(float DeltaSeconds)
{
	if (PlayerOne && PlayerTwo)
	{
		FVector P1L = PlayerOne->GetActorLocation();
		FVector P2L = PlayerTwo->GetActorLocation();
		FVector MidPoint = (P1L + P2L) * 0.5f;
		MidPoint.Z += VerticalOffset;
		MidPoint.Y = PlayerOne->GetDistanceTo(PlayerTwo);
		SetActorLocation(MidPoint);
		float Pitch = FMath::RadiansToDegrees(FMath::Atan2(-VerticalOffset * 0.5f, SpringArm->TargetArmLength));
		SetActorRotation(FRotator(Pitch, FMath::RadiansToDegrees(FMath::Atan2(P1L.Y - P2L.Y, P1L.X - P2L.X)) + 90.0f, 0.0f));
	}
}
