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
	RealColliderActorRef = nullptr;
	// ...
}


// Called when the game starts
void UAnimationColliderComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwner()->GetAllChildActors(ChildActor, false);
	Owner = Cast<AFGDefaultPawn>(GetOwner());

	RealColliderActorRef = Cast<UChildActorComponent>(Owner->GetMesh()->GetChildComponent(0));
	ColliderHolderRef = Cast<UChildActorComponent>(Owner->GetMesh()->GetChildComponent(1));
	// ...
	if (!RealColliderActorRef->GetChildActor()) {
		GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Red, TEXT("NoChildActor"));
	}
}


// Called every frame
void UAnimationColliderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAnimationColliderComponent::DeOrActivateComponents(UHitBoxIDComp* ColliderIdComponent)
{
	UHitBoxIDComp* RealCollID;
	TArray<USceneComponent*> sceneCompHolder;
	TArray<UMyHitBoxComponent*> realHitBoxes;

	RealCollID = Cast<UHitBoxIDComp>(RealColliderActorRef->GetChildActor()->GetComponentsByClass(UHitBoxIDComp::StaticClass())[0]);
	RealCollID->GetChildrenComponents(false, sceneCompHolder);

	for (int i = 0; i < sceneCompHolder.Num(); ++i)
	{
		realHitBoxes.Add(Cast<UMyHitBoxComponent>(sceneCompHolder[i]));
	}

	sceneCompHolder = TArray<USceneComponent*>();
	ColliderIdComponent->GetChildrenComponents(false, sceneCompHolder);

	//hitCollider on 0
	bool Coll = false;
	for (int i = 0; i < sceneCompHolder.Num(); ++i)
	{
		if (Cast<UMyHitBoxComponent>(ColliderIdComponent->GetChildComponent(i))->Etype == EBoxType::Hit)
		{
			realHitBoxes[0]->SetRelativeTransform(Cast<UMyHitBoxComponent>(sceneCompHolder[i])->GetRelativeTransform());
			realHitBoxes[0]->SetBoxExtent(Cast<UMyHitBoxComponent>(sceneCompHolder[i])->GetScaledBoxExtent());

			Coll = true;
			break;
		}
	}
	if (!Coll)
	{
		realHitBoxes[0]->SetRelativeLocation(FVector(0, 0, -2000));
		realHitBoxes[0]->SetBoxExtent(FVector::ZeroVector);
	}

	//blockCollider on 1
	Coll = false;
	for (int i = 0; i < sceneCompHolder.Num(); ++i)
	{
		if (Cast<UMyHitBoxComponent>(ColliderIdComponent->GetChildComponent(i))->Etype == EBoxType::Block)
		{
			realHitBoxes[1]->SetRelativeTransform(Cast<UMyHitBoxComponent>(sceneCompHolder[i])->GetRelativeTransform());
			realHitBoxes[1]->SetBoxExtent(Cast<UMyHitBoxComponent>(sceneCompHolder[i])->GetScaledBoxExtent());
			Coll = true;
			break;
		}
	}
	if (!Coll)
	{
		realHitBoxes[1]->SetRelativeLocation(FVector(0, 0, -2000));
		realHitBoxes[1]->SetBoxExtent(FVector::ZeroVector);
	}

	//hurtCollider 2 to 12
	int counter = 2;
	for (int i = 0; i < sceneCompHolder.Num(); ++i)
	{
		if (Cast<UMyHitBoxComponent>(ColliderIdComponent->GetChildComponent(i))->Etype == EBoxType::Hurt)
		{
			realHitBoxes[counter]->SetRelativeTransform(Cast<UMyHitBoxComponent>(sceneCompHolder[i])->GetRelativeTransform());
			realHitBoxes[counter]->SetBoxExtent(Cast<UMyHitBoxComponent>(sceneCompHolder[i])->GetScaledBoxExtent());

			++counter;
		}
	}

	for (int i = ++counter; i < realHitBoxes.Num(); ++i)
	{
		realHitBoxes[i]->SetRelativeLocation(FVector(0, 0, -2000));
		realHitBoxes[i]->SetBoxExtent(FVector::ZeroVector);

	}
}

void UAnimationColliderComponent::StartAnim(UFGMove * CurrentMove)
{
	if (!RealColliderActorRef && !ColliderHolderRef)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Green, TEXT("FUCK"));
		return;
	}

	this->m_state = 0;

	if (Cast<AFGDefaultPawn>(GetOwner()))
	{
		if (m_MoveToCollider.Find(CurrentMove))
		{
			ColliderHolderRef->SetChildActorClass(*m_MoveToCollider.Find(CurrentMove));
			DeOrActivateComponents(Cast<UHitBoxIDComp>(ColliderHolderRef->GetChildActor()->GetComponentsByClass(UHitBoxIDComp::StaticClass())[this->m_state]));
		}
	}
}


void UAnimationColliderComponent::NextColliderSetup()
{
	++m_state;
	if (!Owner)
	{
		return;
	}
	if (Owner->gotHit)
	{
		if (RealColliderActorRef)
		{
			//RealColliderActorRef->DestroyChildActor();
			return;
		}
	}

	if (RealColliderActorRef)
	{
		if (RealColliderActorRef->GetChildActor()) {

			DeOrActivateComponents(Cast<UHitBoxIDComp>(ColliderHolderRef->GetChildActor()->GetComponentsByClass(UHitBoxIDComp::StaticClass())[FMath::Clamp(this->m_state, 0, ColliderHolderRef->GetChildActor()->GetComponentsByClass(UHitBoxIDComp::StaticClass()).Num() - 1)]));

			//DeOrActivateComponents(Cast<UHitBoxIDComp>(RealColliderActorRef->GetChildActor()->GetComponentsByClass(UHitBoxIDComp::StaticClass())[this->m_state]));
		}
	}
}


void UAnimationColliderComponent::RemoveCurrentChildActor()
{
	if (RealColliderActorRef)
	{
		//RealColliderActorRef->DestroyChildActor();
	}
}
#pragma optimize("", on)
