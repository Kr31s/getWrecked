// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHitBoxComponent.h"
#include "FGDefaultPawn.h"

UMyHitBoxComponent::UMyHitBoxComponent() {

	Etype = EBoxType::Hurt;
	this->ShapeColor = FColor::Green;

	//this->OnComponentBeginOverlap.AddDynamic(this, &UMyHitBoxComponent::CollisionEvent);
}

#if WITH_EDITOR
void UMyHitBoxComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr)
	{

		switch (Etype)
		{
		case EBoxType::Hit:
			this->ShapeColor = FColor::Red;
			break;
		case EBoxType::Block:
			this->ShapeColor = FColor::Blue;
			break;
		default:
			this->ShapeColor = FColor::Green;

			break;

		}
	}
}


#endif
// physical collider Missing capsule not a good choice, because of resizing 

// hurt Collision --> the target where everything is applied  
// hit Collision applies damage
// block Collision --> opponent can block react 


void UMyHitBoxComponent::CollisionEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	 
	auto* collider{ Cast<UMyHitBoxComponent>(OtherComp) };
	/*
	switch (Etype)
	{
	case EBoxType::Hit:
		if (collider->Etype == EBoxType::Hurt)
		{
			auto* opponent = Cast<AFGDefaultPawn>(collider->GetOwner());
			opponent->RessourceComp->ReduceHealth(Cast<AFGDefaultPawn>(this->GetOwner())->GetCurrentMove()->DamageValue);
		}
		break;
	case EBoxType::Block:
		if (collider->Etype == EBoxType::Hurt)
		{

		}
		break;
	default:
		break;
	}
	 */



}

