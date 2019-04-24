// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "MyHitBoxComponent.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EBoxType : uint8 {
	Hurt UMETA(DisplayName = "HurtBox"),
	Hit UMETA(DisplayName = "HitBox"),
	Block UMETA(DisplayName = "BlockBox")
};

UCLASS(ClassGroup = (Collision), meta = (BlueprintSpawnableComponent))
class MYSTATEMACHPRO_API UMyHitBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UMyHitBoxComponent();

	UPROPERTY(EditAnywhere, Category = "Body")
		uint8 someValue = 1;

	UPROPERTY(EditAnywhere, Category = "Body")
		EBoxType Etype;




#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
#endif



};

