// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoFrameActor.generated.h"

UCLASS()
class MYSTATEMACHPRO_API ADemoFrameActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADemoFrameActor();

protected:
	// Called when the game starts or when spawned

	UPROPERTY(EditAnywhere)
		uint8 id;
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
