// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "SM_State.h"
#include "Public/RessourceComponent.h"
#include "PlayerComponents/MovementRestrictionComponent.h"
#include "PlayerComponents/ActorRotationComponent.h"
#include "PlayerComponents/HBParentComp.h"
#include <bitset>
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCurveFloat* DiagonalCurve;

	UPROPERTY(EditAnywhere)
		bool CanMoveInLeftDirection;

	UPROPERTY(EditAnywhere)
		float stunTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Id;

	UPROPERTY(EditAnywhere)
		bool CanMoveInRightDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanBlock;

	UFUNCTION()
	void SetCanBlock(bool blockState) { this->bCanBlock = blockState; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsBlocking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool gotHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool canApplyDamage;

	UPROPERTY(EditAnywhere)
		bool isOnLeftSide;

	UPROPERTY(EditAnywhere)
		bool isStunned;

	// jump Variables 
	UPROPERTY()
		bool jumpInitializeFlag = false;

	UPROPERTY()
		float timeInJump;	
	
	UPROPERTY(Editanywhere, BlueprintReadWrite)
		float jumpDuration = 1.0F;

	UPROPERTY(Editanywhere, BlueprintReadWrite)
		float jumpHeight = 275.0F;

	UPROPERTY(Editanywhere, BlueprintReadWrite)
		float jumpDistance = 275.0F;

	UPROPERTY(Editanywhere, BlueprintReadWrite)
		bool doJump;

	UPROPERTY()
		float directionmodifier;
	
	UPROPERTY()
		FVector jumpTargetLocation;

	UPROPERTY()
		FVector jumpStartLocation;

	UPROPERTY(BlueprintReadOnly)
		AActor* Opponent;

	UPROPERTY()
		TArray<AActor*> ColliderParentsArray;

	UPROPERTY(EditAnywhere)
		TMap<UFGMove*, FString> MoveColliderParents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		URessourceComponent* RessourceComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UMovementRestrictionComponent* MovementRestrictionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UActorRotationComponent* PlayerRotationComp;

	UPROPERTY(EditAnywhere)
		UParticleSystem* gotHitFire;

	UFUNCTION(BlueprintCallable)
		UFGMove* GetCurrentMove();

	void MoveColliderSwitch();
	//UFUNCTION()
	//	void ReadInputstream();

protected:
	void LeftButtonPressed();
	void LeftButtonReleased();
	void TopButtonPressed();
	void TopButtonReleased();
	void RightButtonPressed();
	void RightButtonReleased();
	void BottomButtonPressed();
	void BottomButtonReleased();
	void TriggerLeftPressed();
	void TriggerLeftReleased();
	void TriggerRightPressed();
	void TriggerRightReleased();
	void BumperLeftPressed();
	void BumperLeftReleased();
	void BumperRightPressed();
	void BumperRightReleased();
	void ReadXAxis(float Value);
	void ReadYAxis(float Value);

	void UseGameCamera();

	FVector2D DirectionInput;
	uint32 ButtonsDown;
	uint32 ButtonsDown_Old;
	float TimeInCurrentMove;


	UPROPERTY(EditAnywhere)
	UFGMove* CurrentMove;

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

	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void SetRotationOfPlayer();

private:
	//~ This array relates to InputStream. InputStream must not be updated without this stream being updated as well.
	UPROPERTY(VisibleInstanceOnly)
	TArray<float> InputTimeStamps;

	UPROPERTY(VisibleInstanceOnly)
	TArray<USM_InputAtom*> InputStream;

	std::bitset<12> SendInputStream;

	UFUNCTION()
		void HandleStun(float deltaSeconds);

	void CrouchValues(bool inCrouch);

	UFUNCTION(BlueprintCallable)
	void DiagonalJump(float direction, FVector position, float time, float Height, float Distance);
	//UPROPERTY(VisibleInstanceOnly)
	//TArray<USM_InputAtom*> RecievedInputStream;
};