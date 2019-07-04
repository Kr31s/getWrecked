// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FGDefaultPawn.h"
#include "HitBoxIDComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSTATEMACHPRO_API UHitBoxIDComp : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHitBoxIDComp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MoveID;

	UPROPERTY()
		AFGDefaultPawn* player;

	UFUNCTION(BlueprintCallable)
		const int GetMoveID() { return MoveID; }


	UPROPERTY()
		TArray<USceneComponent*> ColliderComponents;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
