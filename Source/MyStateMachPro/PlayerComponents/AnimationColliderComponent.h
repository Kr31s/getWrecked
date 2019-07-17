// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ChildActorComponent.h"
#include "FGMove.h"
#include "HitBoxIDComp.h"
#include "AnimationColliderComponent.generated.h"

class ADemoFrameActor;

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSTATEMACHPRO_API UAnimationColliderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnimationColliderComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int m_state;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TMap<UFGMove*, TSubclassOf<AActor>> m_MoveToCollider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UChildActorComponent* RealColliderActorRef;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UChildActorComponent* ColliderHolderRef;
	 
	UPROPERTY()
		TArray<AActor*> ChildActor;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
		AFGDefaultPawn* Owner;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void StartAnim(UFGMove* CurrentMove);
	
	UFUNCTION(BlueprintCallable)
		void NextColliderSetup();

	UFUNCTION(BlueprintCallable)
		void DeOrActivateComponents(UHitBoxIDComp* ColliderIdComponent);

	UFUNCTION(BlueprintCallable)
		void RemoveCurrentChildActor();
};
