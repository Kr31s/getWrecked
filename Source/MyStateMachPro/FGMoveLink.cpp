// Fill out your copyright notice in the Description page of Project Settings.

#include "FGMoveLink.h"
#include "FGDefaultPawn.h"
#include "MyStateMachPro.h"
#include "FGState.h"

FStateMachineResult UFGMoveLink::TryLink(const AFGDefaultPawn* RefObject, const TArray<USM_InputAtom*>& DataSource, int32 DataIndex, int32 RemainingSteps)
{
	if (InputStateMachine && Move)
	{
		bool bCanCancel = false;
		//if (RefObject->doJump)
		//{
		//	if (RefObject->GetMovementComponent()->IsMovingOnGround())
		//	{
		//		GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Black, TEXT("OnGround"));

		//	}
		//	if (RefObject->timeInJump > 0.5F)
		//	{
		//		bCanCancel = true;
		//		GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Black, TEXT("CancelAirmoveOnHittingGround"));

		//		return InputStateMachine->RunState(RefObject, DataSource, DataIndex, RemainingSteps);

		//	}
		//}
		for (const FVector2D& CancelWindow : CancelWindows)
		{
			if (RefObject->GetTimeInMove() == FMath::Clamp(RefObject->GetTimeInMove(), CancelWindow.X, CancelWindow.Y))
			{
				bCanCancel = true;
				break;
			}
		}

		//if (RefObject->gotHit)
		//{
		//	return FStateMachineResult();
		//}
		// If no cancel windows are provided, assume the move is always available.
		if (bCanCancel || !CancelWindows.Num())
		{
			return InputStateMachine->RunState(RefObject, DataSource, DataIndex, RemainingSteps);
		}
	}
	return FStateMachineResult();
}