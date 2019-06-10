// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FGMove.h"
#include "HBParentComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class MYSTATEMACHPRO_API UHBParentComp : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHBParentComp();



	UPROPERTY(EditAnywhere)
		USkeletalMesh* PlayerMesh;

	UPROPERTY(EditAnywhere)
		UAnimationAsset* Animation;

	UPROPERTY()
		AFGDefaultPawn* player;

	//CollisionSetupsPerMove
	UPROPERTY()
		TArray<USceneComponent*> TIDChildren;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	void SwitchBetweenChildren();
};
