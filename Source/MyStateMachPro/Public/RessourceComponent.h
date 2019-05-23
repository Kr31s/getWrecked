// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RessourceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSTATEMACHPRO_API URessourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URessourceComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 StunMeter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 PowerMeter;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
