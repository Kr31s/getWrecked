// Fill out your copyright notice in the Description page of Project Settings.

#include "FGInputBranch.h"
#include "FGDefaultPawn.h"
#include "MyStateMachPro.h"

USM_State* UFGInputBranch::TryBranch(const UObject* RefObject, const TArray<USM_InputAtom*>& DataSource,
	int32 DataIndex, int32 &OutDataIndex) 
{
	OutDataIndex = DataIndex;
	if (RequiredButtons & ForbiddenButtons/* & RequiredDirections*/)
	{
		UE_LOG(LogTemp, Warning, TEXT("Impassable condition: Required button is also forbidden."));
		return nullptr;
	}

	// Make sure we have at least the minimum data here.
	if ((DataIndex + (int32)EFGInputButtons::Count) >= DataSource.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Not enough data"));
		return nullptr;
	}

	// The first piece of data must be a DirectionalInput
	if (UFGDirectionalInputAtom * DirectionalInput = Cast<UFGDirectionalInputAtom>(DataSource[DataIndex]))
	{
		if (!(AcceptableDirections & (1 << (int32)DirectionalInput->InputDirections)))
		{
			return nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: No directional input at index %i"), DataIndex);
		return nullptr;
	}
	++OutDataIndex;

	bool bRequiredButtonPressed = false;
	for (int32 i = 0; i < (int32)EFGInputButtons::Count; ++i, ++OutDataIndex)
	{
		if (UFGButtonInputAtom * ButtonAtom = Cast<UFGButtonInputAtom>(DataSource[OutDataIndex]))
		{
			if (RequiredButtons & (1 << i))
			{
				if (ButtonAtom->ButtonState == EFGButtonState::JustPressed)
				{
					bRequiredButtonPressed = true;
					continue;
				}
				else if (ButtonAtom->ButtonState == EFGButtonState::HeldDown)
				{
					continue;
				}
				return nullptr;
			}
			else if (ForbiddenButtons & (1 << i))
			{
				// Any state other than having just pressed the forbidden button is OK, even holding it down.
				if (ButtonAtom->ButtonState != EFGButtonState::JustPressed)
				{
					continue;
				}
				return nullptr;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error: Expected %i button inputs, only found %i."), (int32)EFGInputButtons::Count, i);
			return nullptr;
		}
	}
	//for (int32 i = 0; i < (int32)EFGInputDirections::UpForward; ++i, ++OutDataIndex)
	//{
	//	if (UFGDirectionalInputAtom * DirectionButtonAtom = Cast<UFGDirectionalInputAtom>(DataSource[OutDataIndex]))
	//	{
	//		if (RequiredButtons & (1 << i))
	//		{
	//			if (DirectionButtonAtom->DirectionButtonState == EFGButtonState::JustPressed)
	//			{
	//				bRequiredButtonPressed = true;
	//				continue;
	//			}
	//			else if (DirectionButtonAtom->DirectionButtonState == EFGButtonState::HeldDown)
	//			{
	//				continue;
	//			}
	//			return nullptr;
	//		}
	//		else if (ForbiddenButtons & (1 << i))
	//		{
	//			// Any state other than having just pressed the forbidden button is OK, even holding it down.
	//			if (DirectionButtonAtom->DirectionButtonState != EFGButtonState::JustPressed)
	//			{
	//				continue;
	//			}
	//			return nullptr;
	//		}
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Error: Expected %i directionButton inputs, only found %i."), (int32)EFGInputDirections::UpForward, i);
	//		return nullptr;
	//	}
	//}

	// If there were any required buttons, make sure at least one of them was just pressed.
	if ((RequiredButtons/* || RequiredDirections*/) && !bRequiredButtonPressed)
	{
		return nullptr;
	}
	/**/
	return DestinationState;
}