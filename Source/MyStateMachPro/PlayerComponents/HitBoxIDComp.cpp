// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBoxIDComp.h"

// Sets default values for this component's properties
UHitBoxIDComp::UHitBoxIDComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHitBoxIDComp::BeginPlay()
{
	Super::BeginPlay();
	player = Cast<AFGDefaultPawn>(GetOwner());
	GetChildrenComponents(false, ColliderComponents);
	// ...
	
}


// Called every frame
void UHitBoxIDComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Red, TEXT("HITBOXIDCOMP"));

	// ...
	if(this->IsActive())
	{
		for (USceneComponent* collider : ColliderComponents)
		{
			Cast<UPrimitiveComponent>(collider)->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}else
	{
		for (USceneComponent* collider : ColliderComponents)
		{
			Cast<UPrimitiveComponent>(collider)->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

