// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//#include "GameFramework/Character.h"
#include "MovementRestrictionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSTATEMACHPRO_API UMovementRestrictionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMovementRestrictionComponent();

	UPROPERTY()
		AActor* Self;

	UPROPERTY()
		AActor* Enemy;

	UPROPERTY(EditAnywhere)
		float LeftCornerYValue;

	UPROPERTY(EditAnywhere)
		float MaxDistanceFromMiddle;

	UPROPERTY(EditAnywhere)
		float LeftMapEnd = -900.0F;

	UPROPERTY(EditAnywhere)
		float RightMapEnd = 900.0F;

	UPROPERTY(EditAnywhere)
		float YValueForPlayer;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	void TickComponent();
	void PositionToBorderCheck();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
