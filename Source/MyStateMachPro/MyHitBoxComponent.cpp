// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHitBoxComponent.h"

UMyHitBoxComponent::UMyHitBoxComponent() {

	Etype = EBoxType::Hurt;
	this->ShapeColor = FColor::Green;


}

#if WITH_EDITOR
	void UMyHitBoxComponent::PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent) 
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


