// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "FGDefaultPawn.h"
#include "MyHitBoxComponent.generated.h"

/**
 * try to use here forward declaration
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
	virtual void PostInitProperties() override;

	UPROPERTY(EditAnywhere, Category = "Body")
		uint8 someValue = 1;


	UPROPERTY(EditAnywhere,BlueprintReadWrite , Category = "Body")
		EBoxType Etype;


	UFUNCTION(BlueprintCallable)
		void CollisionEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void CollisionEndEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                       int32 OtherBodyIndex);

	UPROPERTY(BlueprintReadWrite)
		AFGDefaultPawn* Owner;

	UPROPERTY(BlueprintReadWrite)
		AFGDefaultPawn* Enemy;
private:

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
#endif


};

