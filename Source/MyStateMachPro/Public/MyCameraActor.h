// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "MyCameraActor.generated.h"

/**
 * 
 */

class AFGDefaultPawn;

UCLASS()
class MYSTATEMACHPRO_API AMyCameraActor : public ACameraActor
{
	GENERATED_BODY()

public:
	AMyCameraActor();
	virtual void Tick(float DeltaSeconds) override;

	//virtual void BeginPlay() override;

	UPROPERTY()
		float camInterpolation;

	UPROPERTY()
		AFGDefaultPawn* PlayerOne;

	UPROPERTY()
		AFGDefaultPawn* PlayerTwo;

	UPROPERTY(EditAnywhere)
		float YValueHowCloseToPlayer;

	UPROPERTY()
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBoxComponent* LeftEdgeBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBoxComponent* RightEdgeBox;

	
	UPROPERTY(EditAnywhere)
		float VerticalOffset;

	UPROPERTY()
		FVector OldMidPoint;

	UPROPERTY(EditAnywhere)
		float ZDistanceModifier;

	UPROPERTY(EditAnywhere)
		float InterpModifier;

	UPROPERTY(EditAnywhere)
		float YawRotaModifier;

};
