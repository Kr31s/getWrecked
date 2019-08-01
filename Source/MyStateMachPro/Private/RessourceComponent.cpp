// Fill out your copyright notice in the Description page of Project Settings.


#include "RessourceComponent.h"
#include "FGDefaultPawn.h"

// Sets default values for this component's properties
URessourceComponent::URessourceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Health = 1.0F;
	StunMeter = 0.0F;
	PowerMeter = 0;
	
	// ...
}


// Called when the game starts
void URessourceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void URessourceComponent::ReduceHealth(float damageValue)
{
	this->Health -= damageValue;
	OnHealthChanged.Broadcast(GetOwner(),this->Health);
}

void URessourceComponent::IncreaseStunMeter(float value)
{
	StunMeter += value;
	OnStunMeterChanged.Broadcast(GetOwner(),this->StunMeter);
	if (StunMeter >= 1.0F)
	{
		Cast<AFGDefaultPawn>(GetOwner())->isStunned = true;
		SetStunMeter(0.0F);
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Emerald, TEXT("StunOver"));
		//Cast<AFGDefaultPawn>(GetOwner())->isStunned = false;
	}

}

void URessourceComponent::IncreasePowerMeter(float value)
{
	PowerMeter = FMath::Clamp(PowerMeter+=value, 0.0F, 1.0F);
	OnPowerMeterChanged.Broadcast(GetOwner(), this->PowerMeter);
}
// Called every frame
void URessourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(Cast<AFGDefaultPawn>(GetOwner())->gotHit)
	{
		stunMeterDecayTimer = 0;
	}else
	{
		stunMeterDecayTimer += DeltaTime;
		if (StunMeter >= 0.01F && stunMeterDecayTimer > stunMeterDecayMaxTime) {
			IncreaseStunMeter(-0.001F);
		}
	}

	// ...
}

