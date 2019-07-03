// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationColliderComponent.h"
#include "DemoFrameActor.h"

// Sets default values for this component's properties
UAnimationColliderComponent::UAnimationColliderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAnimationColliderComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAnimationColliderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAnimationColliderComponent::DeOrActivateComponents()
{
}


void UAnimationColliderComponent::StartAnim(UFGMove* CurrentMove)
{
	this->m_state = 0;
	auto* owner = Cast<AFGDefaultPawn>(GetOwner());
	if(owner)
	{
		if(m_MoveToCollider.Find(owner->GetCurrentMove()))
		{
			ADemoFrameActor* temp = *m_MoveToCollider.Find(owner->GetCurrentMove());


			//ChildColliderActorRef->SetChildActorClass(dynamic_cast<TSubclassOf<AActor>>(temp));


			ChildColliderActorRef->GetChildActor()->GetComponentsByClass(UHitBoxIDComp::StaticClass());
			
		}
		
	}
}

void UAnimationColliderComponent::NextColliderSetup(TArray<UHitBoxIDComp*> ColliderIdComponents, int moveId)
{

}

void UAnimationColliderComponent::RemoveCurrentChildActor()
{
	ChildColliderActorRef->SetChildActorClass(nullptr);
}

 
