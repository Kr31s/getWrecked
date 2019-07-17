// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCameraActor.h"
#include "MyStateMachPro.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "FGDefaultPawn.h"

AMyCameraActor::AMyCameraActor()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring"));
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	SpringArm->SetupAttachment(RootComponent);
	GetCameraComponent()->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	YValueHowCloseToPlayer = 300.0;
	VerticalOffset = 70.0f;
	ZDistanceModifier = 25;
	InterpModifier = 0.5F;
	YawRotaModifier = 50;
	//FieldOfView = 60;
	this->GetCameraComponent()->SetFieldOfView(60.0F);
	PrimaryActorTick.bCanEverTick = true;
	SetActorRotation(FRotator(-5.0F,-90.0F,0.0F));
	SetActorLocation(FVector(115.0F,300.0F,162.0F));
}

//void AMyCameraActor::BeginPlay() {}


void AMyCameraActor::Tick(float DeltaSeconds)
{
	if (PlayerOne && PlayerTwo)
	{
		FVector P1L = PlayerOne->GetActorLocation();
		FVector P2L = PlayerTwo->GetActorLocation();
		FVector MidPoint = (P1L + P2L) * 0.5f;
		float playerDistance = PlayerOne->GetDistanceTo(PlayerTwo);
		MidPoint.Z += VerticalOffset;
		MidPoint.Y = YValueHowCloseToPlayer;
		SetActorLocation(MidPoint);

		float Pitch = FMath::RadiansToDegrees(FMath::Atan2(-VerticalOffset * 0.5f, SpringArm->TargetArmLength));
		float ZDistance = FMath::Abs(P1L.Z - P2L.Z)/ ZDistanceModifier;
		
		
		//Rotation Calculations
		FRotator Target = UKismetMathLibrary::FindLookAtRotation(P1L, PlayerOne->GetVelocity() + P1L);
		FRotator TargetRota = FMath::RInterpTo(GetActorRotation(), FRotator(/*Pitch*/ + ZDistance/*InPitch*/, (-90.0f + PlayerOne->GetVelocity().X/ YawRotaModifier) + (PlayerTwo->GetVelocity().X / YawRotaModifier)/*InYaw*/, 0.0f/*InRoll*/), DeltaSeconds, InterpModifier);
		
		//SetActorRotation(FRotator(Pitch + ZDistance/*InPitch*/, (- 90.0f)/*InYaw*/, 0.0f/*InRoll*/));
		
		SetActorRotation(TargetRota);
		float distance = FMath::Abs(P1L.X - P2L.X);
		GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Red, FString::SanitizeFloat(distance));
		if(distance > 85.0F)
		{
			PlayerOne->bCollisionWithOppenent = false;
			PlayerTwo->bCollisionWithOppenent = false;
		}
		if(PlayerOne->bCollisionWithOppenent && PlayerTwo->bCollisionWithOppenent && !PlayerOne->doJump && !PlayerTwo->doJump)
		{
			if((PlayerOne->GetVelocity().X > 340 || PlayerOne->GetVelocity().X < -340) && (PlayerTwo->GetVelocity().X > 340 || PlayerTwo->GetVelocity().X < -340)){
				return;
			}
			PlayerOne->SetActorLocation(PlayerOne->GetActorLocation() + FVector(FMath::Sign(PlayerTwo->GetVelocity().X) * 3.0F, 0, 0));
			PlayerTwo->SetActorLocation(PlayerTwo->GetActorLocation() + FVector(FMath::Sign(PlayerOne->GetVelocity().X) * 3.0F, 0, 0));
		}
	}
}




