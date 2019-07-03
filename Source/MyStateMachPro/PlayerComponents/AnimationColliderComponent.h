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
		AActor* m_ColliderActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TMap<UFGMove*, ADemoFrameActor*> m_MoveToCollider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UChildActorComponent* ChildColliderActorRef;
	 

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void StartAnim(UFGMove* CurrentMove);
	
	UFUNCTION(BlueprintCallable)
		void NextColliderSetup(TArray<UHitBoxIDComp*> ColliderIdComponents, int moveId);

	UFUNCTION(BlueprintCallable)
		void DeOrActivateComponents();

	UFUNCTION()
		void RemoveCurrentChildActor();
};
