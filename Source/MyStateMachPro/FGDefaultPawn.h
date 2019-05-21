// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "SM_State.h"
#include "Public/RessourceComponent.h"
#include "PlayerComponents/MovementRestrictionComponent.h"
#include "PlayerComponents/ActorRotationComponent.h"

#include "FGDefaultPawn.generated.h"


class UFGDirectionalInputAtom;
class UFGButtonInputAtom;
class UFGMove;
/**
 * 
 */
UCLASS()
class MYSTATEMACHPRO_API AFGDefaultPawn : public ACharacter
{
	GENERATED_BODY()
	
public: 
	AFGDefaultPawn();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

	FORCEINLINE float GetTimeInMove() const { return TimeInCurrentMove; }



protected:
	void LeftButtonPressed();
	void LeftButtonReleased();
	void TopButtonPressed();
	void TopButtonReleased();
	void RightButtonPressed();
	void RightButtonReleased();
	void BottomButtonPressed();
	void BottomButtonReleased();
	void ReadXAxis(float Value);
	void ReadYAxis(float Value);

	void UseGameCamera();

	FVector2D DirectionInput;
	uint32 ButtonsDown;
	uint32 ButtonsDown_Old;
	float TimeInCurrentMove;

	UPROPERTY(EditAnywhere)
	UFGMove* CurrentMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		URessourceComponent* RessourceComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UMovementRestrictionComponent* MovementRestrictionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UActorRotationComponent* PlayerRotationComp;

	UPROPERTY(VisibleAnywhere)
		UBoxComponent* PunchL;	
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* PunchR;	
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* KickL;	
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* KickR;
	// Input atoms are removed when they pass this age threshold. All moves must be executed in under this time.
	UPROPERTY(EditAnywhere)
	float InputExpirationTime;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
	UFGDirectionalInputAtom* DirectionDownBackAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
	UFGDirectionalInputAtom* DirectionDownAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
	UFGDirectionalInputAtom* DirectionDownForwardAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
	UFGDirectionalInputAtom* DirectionBackAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
	UFGDirectionalInputAtom* DirectionNeutralAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
	UFGDirectionalInputAtom* DirectionForwardAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
	UFGDirectionalInputAtom* DirectionUpBackAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
	UFGDirectionalInputAtom* DirectionUpAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
	UFGDirectionalInputAtom* DirectionUpForwardAtom;

	// Order in this array is the same as EFGButtonState: Up, JustPressed, Held
	UPROPERTY(EditAnywhere, Category = "Input Atoms")
	TArray<UFGButtonInputAtom*> ButtonAtoms;

	UFUNCTION(BlueprintImplementableEvent)
	void DoMove(UFGMove* NewMove);

	UFUNCTION(/*BlueprintNativeEvent*/)
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	//~ This array relates to InputStream. InputStream must not be updated without this stream being updated as well.
	UPROPERTY(VisibleInstanceOnly)
	TArray<float> InputTimeStamps;

	UPROPERTY(VisibleInstanceOnly)
	TArray<USM_InputAtom*> InputStream;
};
