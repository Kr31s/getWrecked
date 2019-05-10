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

	UPROPERTY()
		AActor* PlayerOne;

	UPROPERTY()
		AActor* PlayerTwo;

	UPROPERTY()
		USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
		float VerticalOffset;
};
