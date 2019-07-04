// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MoveIDNotifier.generated.h"

/**
 * 
 */
UCLASS()
class MYSTATEMACHPRO_API UMoveIDNotifier : public UAnimNotify
{
	GENERATED_BODY()

public:
	UMoveIDNotifier();

	UPROPERTY(EditAnywhere)
		int Id;


};
