// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyCameraActor.generated.h"

/**
 * 
 */
UCLASS()
class MYSTATEMACHPRO_API AMyCameraActor : public ACameraActor
{
	GENERATED_BODY()

public:
	AMyCameraActor();
	virtual void Tick(float DeltaSeconds) override;

	void EdgeMovementRestriction();
	//virtual void BeginPlay() override;

	UPROPERTY()
		AActor* PlayerOne;

	UPROPERTY()
		AActor* PlayerTwo;

	UPROPERTY(EditAnywhere)
		float LeftCornerYValue;

	UPROPERTY(EditAnywhere)
		float RightCornerYValue;

	UPROPERTY()
		USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
		float VerticalOffset;

	UPROPERTY(EditAnywhere)
		float ZDistanceModifier;

	UPROPERTY(EditAnywhere)
		float InterpModifier;

	UPROPERTY(EditAnywhere)
		float YawRotaModifier;
};
