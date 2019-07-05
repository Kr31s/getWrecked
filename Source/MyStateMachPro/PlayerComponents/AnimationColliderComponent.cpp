// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationColliderComponent.h"
#include "DemoFrameActor.h"
#include "MyHitBoxComponent.h"

#pragma optimize("", off)
// Sets default values for this component's properties
UAnimationColliderComponent::UAnimationColliderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ChildColliderActorRef = nullptr;
	// ...
}


// Called when the game starts
void UAnimationColliderComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwner()->GetAllChildActors(ChildActor, false);
	Owner = Cast<AFGDefaultPawn>(GetOwner());
	
	ChildColliderActorRef = Cast<UChildActorComponent>(Owner->GetMesh()->GetChildComponent(0));
	// ...
	
}


// Called every frame
void UAnimationColliderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAnimationColliderComponent::DeOrActivateComponents(TArray<UHitBoxIDComp*> ColliderIdComponents, int moveId)
{
	for(UHitBoxIDComp* IDs : ColliderIdComponents)
	{
		if(Owner->gotHit)
		{
			if (ChildColliderActorRef)
			{
				ChildColliderActorRef->DestroyChildActor();
				return;
			}

		}

		if(IDs->GetMoveID() == moveId)
		{
			IDs->SetActive(true);
			TArray<USceneComponent*> NotCastedColliderPerID;
			IDs->GetChildrenComponents(false, NotCastedColliderPerID);
			TArray<UPrimitiveComponent*> ColliderPerID;
			TArray<UMyHitBoxComponent*> CastedToBoxColliderPerID;
			for (USceneComponent* Collider : NotCastedColliderPerID)
			{
				ColliderPerID.Add(Cast<UPrimitiveComponent>(Collider));
			}
			for (UPrimitiveComponent* Collider : ColliderPerID)
			{
				CastedToBoxColliderPerID.Add(Cast<UMyHitBoxComponent>(Collider));
			}
			for(UMyHitBoxComponent* Collider : CastedToBoxColliderPerID)
			{
				Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				//Collider->AddRelativeLocation(FVector(0.5F, 0.0F, 0.0F));
			}
		}else
		{
			IDs->SetActive(false);
			TArray<USceneComponent*> NotCastedColliderPerID;
			IDs->GetChildrenComponents(false, NotCastedColliderPerID);
			TArray<UPrimitiveComponent*> ColliderPerID;
			for(USceneComponent* Collider : NotCastedColliderPerID)
			{
				ColliderPerID.Add(Cast<UPrimitiveComponent>(Collider));
			}
			for(UPrimitiveComponent* Collider : ColliderPerID)
			{
				Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			
		}
	}


}

void UAnimationColliderComponent::StartAnim(UFGMove* CurrentMove)
{
	if(!ChildColliderActorRef)
	{
		return;
	}

	this->m_state = 0;
	auto* owner = Cast<AFGDefaultPawn>(GetOwner());
	if(owner)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Black, CurrentMove->MoveName.ToString());

		if(m_MoveToCollider.Find(CurrentMove))
		{
			

			//TSubclassOf<AActor> temp = m_MoveToCollider.Find(CurrentMove);


			ChildColliderActorRef->SetChildActorClass(*m_MoveToCollider.Find(CurrentMove));

			TArray<UHitBoxIDComp*> idComponents;
			if (ChildColliderActorRef)
			{
				for (UActorComponent* IDs : ChildColliderActorRef->GetChildActor()->GetComponentsByClass(UHitBoxIDComp::StaticClass()))
				{
					idComponents.Add(Cast<UHitBoxIDComp>(IDs));
				}
				DeOrActivateComponents(idComponents, m_state);
			}
		}
		
	}
}


void UAnimationColliderComponent::NextColliderSetup()
{
	m_state++;
	if (Owner->gotHit)
	{
		if (ChildColliderActorRef)
		{
			ChildColliderActorRef->DestroyChildActor();
			return;
		}
	}
	TArray<UHitBoxIDComp*> idComponentsNext;
	if (ChildColliderActorRef)
	{
		if(AActor* childActor = ChildColliderActorRef->GetChildActor())
		{
			TArray<UActorComponent*> temp = childActor->GetComponentsByClass(UHitBoxIDComp::StaticClass());

			for (UActorComponent* IDs : temp)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Green, TEXT("IFCASE TRUE"));

				idComponentsNext.Add(Cast<UHitBoxIDComp>(IDs));
			}

			DeOrActivateComponents(idComponentsNext, m_state);
		}
	}
}


void UAnimationColliderComponent::RemoveCurrentChildActor()
{
	if (ChildColliderActorRef)
	{
		ChildColliderActorRef->DestroyChildActor();
	}
}
#pragma optimize("", on)
