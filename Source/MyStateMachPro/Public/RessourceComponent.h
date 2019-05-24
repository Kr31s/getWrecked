// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RessourceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) , BlueprintType, Blueprintable)
class MYSTATEMACHPRO_API URessourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URessourceComponent();


	const float GetHealth() { return Health; }
	void SetHealth(float health) { this->Health = health; }
 
	const float GetStunMeter() { return StunMeter; }
	void SetStunMeter(float stunMeter) { this->StunMeter = stunMeter; }

/*
	const float GetPowerMeter() { return PowerMeter; }
	void SetPowerMeter(int32 powerMeter) { this->PowerMeter = powerMeter; }
 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StunMeter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 PowerMeter;

	UFUNCTION(BlueprintCallable)
		void ReduceHealth(float damageValue);

	UFUNCTION(BlueprintCallable)
		void IncreaseStunMeter(float value);

	UFUNCTION(BlueprintCallable)
		void IncreasePowerMeter(AActor* actor, int32 value);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature,AActor*, target, float, newHealth);
	UPROPERTY(BlueprintAssignable)
		FOnHealthChangedSignature OnHealthChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStunMeterChangedSignature, AActor*, target, float, newStunMeter);
	UPROPERTY(BlueprintAssignable)
		FOnStunMeterChangedSignature OnStunMeterChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
