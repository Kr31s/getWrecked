// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCameraActor.h"
#include "MyStateMachPro.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

AMyCameraActor::AMyCameraActor()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring"));
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	SpringArm->SetupAttachment(RootComponent);
	GetCameraComponent()->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	VerticalOffset = 140.0f;
	ZDistanceModifier = 10;
	InterpModifier = 0.5F;
	YawRotaModifier = 50;

	LeftCornerYValue = -300.0F;
	RightCornerYValue = 300.0F;

	PrimaryActorTick.bCanEverTick = true;
}
/*
void AMyCameraActor::BeginPlay() {
	//SetActorRotation(FRotator(-13.0F,-90.0F,0.0F));
	//SetActorLocation(FVector(160.0F,180.0F,232.0F));
}
*/

void AMyCameraActor::Tick(float DeltaSeconds)
{
	if (PlayerOne && PlayerTwo)
	{
		FVector P1L = PlayerOne->GetActorLocation();
		FVector P2L = PlayerTwo->GetActorLocation();
		FVector MidPoint = (P1L + P2L) * 0.5f;
		FVector LeftEdge = FVector(MidPoint.X - RightCornerYValue, 0, 0);
		FVector RightEdge = FVector(MidPoint.X + RightCornerYValue, 0, 0);
		float playerDistance = PlayerOne->GetDistanceTo(PlayerTwo);
		MidPoint.Z += VerticalOffset;
		MidPoint.Y = 180.0F;
		SetActorLocation(MidPoint);
		//LeftEdge->
		if (/*P1L.X <= LeftEdge.X*/playerDistance > RightCornerYValue*2) {
			//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Black, TEXT("Welcome TO THE LeftEDGE"));
			//PlayerOne->SetActorLocation(FVector(LeftEdge.X, P1L.Y, P1L.Z));
			//PlayerTwo->SetActorLocation(FVector(RightEdge.X, P2L.Y, P2L.Z));
		}/*
		else {
		}
			if (P2L.X >= RightEdge.X) {
				
				GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Cyan, TEXT("Welcome TO THE RightEDGE"));
				//PlayerOne->SetActorLocation(FVector(RightEdge.X, P1L.Y, P1L.Z));
				//PlayerTwo->SetActorLocation(FVector(LeftEdge.X, P2L.Y, P2L.Z));

			}
		 */

		float Pitch = FMath::RadiansToDegrees(FMath::Atan2(-VerticalOffset * 0.5f, SpringArm->TargetArmLength));
		float ZDistance = FMath::Abs(P1L.Z - P2L.Z)/ ZDistanceModifier;
		


		
		//Rotation Calculations
		FRotator Target = UKismetMathLibrary::FindLookAtRotation(P1L, PlayerOne->GetVelocity() + P1L);
		FRotator TargetRota = FMath::RInterpTo(GetActorRotation(), FRotator(Pitch + ZDistance/*InPitch*/, (-90.0f + PlayerOne->GetVelocity().X/ YawRotaModifier)/*InYaw*/, 0.0f/*InRoll*/), DeltaSeconds, InterpModifier);
		
		//Camera prints

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::SanitizeFloat(-90.0f + PlayerOne->GetVelocity().X / YawRotaModifier));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(Pitch + ZDistance));
		
		
		//SetActorRotation(FRotator(Pitch + ZDistance/*InPitch*/, (- 90.0f)/*InYaw*/, 0.0f/*InRoll*/));

		SetActorRotation(TargetRota);
	}
}
void AMyCameraActor::EdgeMovementRestriction()
{



}

