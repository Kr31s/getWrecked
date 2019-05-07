// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FGDefaultPawn.h"
#include "Engine/DataAsset.h"
#include "RessourceData.generated.h"


//class AFGDefaultPawn;

/**
 * 
 */
UCLASS()
class MYSTATEMACHPRO_API URessourceData : public UDataAsset
{
	GENERATED_BODY()


public: 

	//URessourceData();

	UPROPERTY(EditAnywhere)
		uint32 Health;
	UPROPERTY(EditAnywhere)
		uint32 StunMeter;
	UPROPERTY(EditAnywhere)
		uint32 PowerMeter;
	/*
	UFUNCTION()
		void TakeDamge(const AFGDefaultPawn* RefObject, uint32 damageValue);

	UFUNCTION()
		void PowerMeterUp(const AFGDefaultPawn* RefObject, uint32 value);

	UFUNCTION()
		void StunMeterUp(const AFGDefaultPawn* RefObject, uint32 value);
	*/
	
};
