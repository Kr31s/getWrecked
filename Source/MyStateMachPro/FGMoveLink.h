// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FGState.h"
//#include "FGMove.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FGMoveLink.generated.h"



class AFGDefaultPawn;
class UFGMove;
/**
 * 
 */
UCLASS()
class MYSTATEMACHPRO_API UFGMoveLink : public UDataAsset
{
	GENERATED_BODY()
	
public: 
	FStateMachineResult TryLink(const AFGDefaultPawn* RefObject, const TArray<USM_InputAtom*>& DataSource, int32 DataIndex = 0, int32 RemainigSteps = - 1);

	UPROPERTY(EditAnywhere)
		UFGMove* Move;

	// Set of windows (in seconds) when this link can be used.
	UPROPERTY(EditAnywhere)
		TArray<FVector2D> CancelWindows;

	UPROPERTY(EditAnywhere)
		bool cancelOnGrounded = false;

	// Clears input buffer when using this link
	UPROPERTY(EditAnywhere)
		uint32 bClearInput : 1;

	// Sequence of inputs used to trigger this move. If not Null, will override move's own input status
	UPROPERTY(EditAnywhere)
		UFGState* InputStateMachine;
};
