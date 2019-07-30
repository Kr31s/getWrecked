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
	SpringArm->bEnableCameraLag = true;


	LeftEdgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftEdgeBox"));
	RightEdgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightEdgeBox"));


	LeftEdgeBox->SetRelativeTransform(FTransform(FVector(1.0F, 1.0F, 1.0F)));
	LeftEdgeBox->SetupAttachment(RootComponent);
	RightEdgeBox->SetupAttachment(RootComponent);
	LeftEdgeBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RightEdgeBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LeftEdgeBox->SetCollisionObjectType(ECC_Vehicle);
	RightEdgeBox->SetCollisionObjectType(ECC_Vehicle);
	LeftEdgeBox->SetBoxExtent(FVector(316.0F, 16.0F, 1000.0F));
	RightEdgeBox->SetBoxExtent(FVector(316.0F, 16.0F, 1000.0F));
	LeftEdgeBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightEdgeBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftEdgeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	RightEdgeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	YValueHowCloseToPlayer = 300.0;
	VerticalOffset = 70.0f;
	ZDistanceModifier = 25;
	InterpModifier = 0.5F;
	YawRotaModifier = 50;
	//FieldOfView = 60;
	this->GetCameraComponent()->SetFieldOfView(60.0F);
	PrimaryActorTick.bCanEverTick = true;
	SetActorRotation(FRotator(0.0F,-90.0F,0.0F));
	SetActorLocation(FVector(0.0F,300.0F,175.0F));
	//this->GetCameraComponent()->
}

//void AMyCameraActor::BeginPlay() {}


void AMyCameraActor::Tick(float DeltaSeconds)
{
	if (PlayerOne && PlayerTwo)
	{
		FVector P1L = PlayerOne->GetActorLocation();
		FVector P2L = PlayerTwo->GetActorLocation();
		FVector MidPoint = (P1L + P2L) * 0.5f;
		if(OldMidPoint == FVector(0,0,0))
		{
			OldMidPoint = (P1L + P2L) * 0.5f;
		}
		
		float cameraLeftEdge = MidPoint.X - (900 / 2);
		float cameraRightEdge = MidPoint.X + (900 / 2);
		LeftEdgeBox->SetWorldLocation(FVector(cameraLeftEdge, MidPoint.Y, GetActorLocation().Z));
		RightEdgeBox->SetWorldLocation(FVector(cameraRightEdge, MidPoint.Y, GetActorLocation().Z));
		float playerDistance = PlayerOne->GetDistanceTo(PlayerTwo);
		MidPoint.Z = FMath::Lerp(MidPoint.Z, OldMidPoint.Z, 0.1F) + VerticalOffset;

		MidPoint.Y = YValueHowCloseToPlayer;
		OldMidPoint.Z = MidPoint.Z;
		SetActorLocation(MidPoint);

		float Pitch = FMath::RadiansToDegrees(FMath::Atan2(-VerticalOffset * 0.5f, SpringArm->TargetArmLength));
		float ZDistance = FMath::Abs(P1L.Z - P2L.Z)/ ZDistanceModifier;
		
		
		//Rotation Calculations
		FRotator Target = UKismetMathLibrary::FindLookAtRotation(P1L, PlayerOne->GetVelocity() + P1L);
		FRotator TargetRota = FMath::RInterpTo(GetActorRotation(), FRotator(/*Pitch*/ + ZDistance/*InPitch*/, (-90.0f + PlayerOne->GetVelocity().X/ YawRotaModifier) + (PlayerTwo->GetVelocity().X / YawRotaModifier)/*InYaw*/, 0.0f/*InRoll*/), DeltaSeconds, InterpModifier);
		
		//SetActorRotation(FRotator(Pitch + ZDistance/*InPitch*/, (- 90.0f)/*InYaw*/, 0.0f/*InRoll*/));
		
		SetActorRotation(TargetRota);
		float distance = FMath::Abs(P1L.X - P2L.X);
		if(distance > 84.0F)
		{
			PlayerOne->bCollisionWithOppenent = false;
			PlayerTwo->bCollisionWithOppenent = false;
		}
		if(PlayerOne->bCollisionWithOppenent && PlayerTwo->bCollisionWithOppenent && !PlayerOne->doJump && !PlayerTwo->doJump)
		{
			if((PlayerOne->GetVelocity().X > 340 || PlayerOne->GetVelocity().X < -340) && (PlayerTwo->GetVelocity().X > 340 || PlayerTwo->GetVelocity().X < -340)){
				return;
			}

			PlayerOne->SetActorLocation(PlayerOne->GetActorLocation() + FVector(FMath::Sign(PlayerTwo->GetVelocity().X) * 2.5F, 0, 0));
			PlayerTwo->SetActorLocation(PlayerTwo->GetActorLocation() + FVector(FMath::Sign(PlayerOne->GetVelocity().X) * 2.5F, 0, 0));
		}
	}
}




