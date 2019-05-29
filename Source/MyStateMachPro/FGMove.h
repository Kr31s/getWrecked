// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FGState.h"
//#include "FGMoveLink.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Animation/AnimMontage.h"
#include "FGMove.generated.h"


class UFGMoveLink;
class AFGDefaultPawn;

UENUM(Meta = (Bitflags))
enum class EMoveType : uint8
{
	HK,
	HP,
	LK,
	LP,
	none

};

USTRUCT()
struct  FFGMoveLinkToFollow {
	GENERATED_BODY()

		FFGMoveLinkToFollow()
	{
		Link = nullptr;
	}

	UPROPERTY()
		UFGMoveLink* Link;

	UPROPERTY()
		FStateMachineResult SMR;
};

/**
 * 
 */
UCLASS(BlueprintType)
class MYSTATEMACHPRO_API UFGMove : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//Tries all links, returns the best one to follow and the reulst of the state machine that got there
	FFGMoveLinkToFollow TryLinks(const AFGDefaultPawn* RefObject, const TArray<USM_InputAtom*>& DataSource, int32 DataIndex = 0, int32 RemainingSteps = -1);

	// Animation to play while doing this move.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
		FName MoveName;
	
	// Animation to play while doing this move.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UAnimMontage* Animation;

	// Links from this move to others.
	UPROPERTY(EditAnywhere)
		TArray<UFGMoveLink*> LinkedMoves;

	// Clears input buffer when entering this state.
	UPROPERTY(EditAnywhere)
		uint32 bClearInputOnEntry : 1;

	// Clears input buffer when exiting this state.
	UPROPERTY(EditAnywhere)
	uint32 bClearInputOnExit : 1;

	UPROPERTY(EditAnywhere)
		float DamageValue = 0.0F;

	UPROPERTY(EditAnywhere)
		EMoveType Movetype;

};




