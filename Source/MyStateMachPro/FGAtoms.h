// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SM_State.h"
#include "FGAtoms.generated.h"


UENUM(Blueprintable, Meta = (Bitflags))
enum class EFGInputDirections :uint8
{
	DownBack,
	Down,
	DownForward,
	Back,
	Neutral,
	Forward,
	UpBack,
	Up,
	UpForward
};

UENUM()
enum class EFGButtonState : uint8
{
	Up,
	JustPressed,
	HeldDown,
	Count UMETA(Hidden)
};

UENUM(Blueprintable, Meta = (Bitflags))
enum class EFGInputButtons : uint8
{
	LeftFace,
	TopFace,
	RightFace,
	BottomFace,
	Count UMETA(Hidden)
};

/**
 * 
 */
UCLASS()
class MYSTATEMACHPRO_API UFGDirectionalInputAtom : public USM_InputAtom
{
	GENERATED_BODY()

public:
	//Which direction we re describing with this atom.
	UPROPERTY(EditAnywhere)
		EFGInputDirections InputDirections;

	UPROPERTY(EditAnywhere)
		EFGButtonState DirectionButtonState;
};

UCLASS()
class MYSTATEMACHPRO_API UFGButtonInputAtom : public USM_InputAtom
{
	GENERATED_BODY()

public:
	// State of the button being described.
	UPROPERTY(EditAnywhere)
	EFGButtonState ButtonState;
};

UCLASS()
class MYSTATEMACHPRO_API UFGAtoms : public USM_InputAtom
{
	GENERATED_BODY()

};

