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
	if (!ChildColliderActorRef->GetChildActor()) {
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
	TArray<AActor*> testo;
	TArray<USceneComponent*> size;
	TArray<UMyHitBoxComponent*> testo2;

	ChildColliderActorRef->GetChildActor()->GetAllChildActors(testo, true);

	for (int i = 0; i < testo.Num(); ++i)
	{
		if (Cast<UHitBoxIDComp>(testo[i]))
		{
			AActor* boom = testo[i];
			boom->GetAllChildActors(testo, false);
		}
	}

	ColliderIdComponent->GetChildrenComponents(false, size);

	for (int i = 0; i < testo.Num(); ++i)
	{
		testo2[i] = Cast<UMyHitBoxComponent>(testo[i]);
	}

	//hitCollider on 0
	bool Coll = false;
	for (int i = 0; i < size.Num(); ++i)
	{
		if (Cast<UMyHitBoxComponent>(ColliderIdComponent->GetChildComponent(i))->Etype == EBoxType::Hit)
		{
			testo2[0] = Cast<UMyHitBoxComponent>(size[i]);
			Coll = true;
			break;
		}
	}
	if (!Coll)
	{
		testo2[0]->SetRelativeLocation(FVector(0, 2000, -2000));
	}

	//blockCollider on 1
	Coll = false;
	for (int i = 0; i < size.Num(); ++i)
	{
		if (Cast<UMyHitBoxComponent>(ColliderIdComponent->GetChildComponent(i))->Etype == EBoxType::Block)
		{
			testo2[1] = Cast<UMyHitBoxComponent>(size[i]);
			Coll = true;
			break;
		}
	}
	if (!Coll)
	{
		testo2[1]->SetRelativeLocation(FVector(0, 2000, -2000));
	}

	//hurtCollider 2 to 12
	int lastiD = 0;
	int counter = 2;
	for (int i = 0; i < size.Num(); ++i)
	{
		if (Cast<UMyHitBoxComponent>(ColliderIdComponent->GetChildComponent(i))->Etype == EBoxType::Hurt)
		{
			testo2[counter] = Cast<UMyHitBoxComponent>(size[i]);
			++counter;
		}
		lastiD = i;
	}

	for (int i = ++lastiD; i < size.Num(); ++i)
	{
		testo2[i]->SetRelativeLocation(FVector(0, 2000, -2000));
	}
}

void UAnimationColliderComponent::StartAnim(UFGMove * CurrentMove)
{
	if (!ChildColliderActorRef)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Green, TEXT("FUCK"));
		return;
	}

	this->m_state = 0;

	if (Cast<AFGDefaultPawn>(GetOwner()))
	{
		if (m_MoveToCollider.Find(CurrentMove))
		{
			DeOrActivateComponents(Cast<UHitBoxIDComp>(Cast<AActor>(m_MoveToCollider.Find(CurrentMove)->GetDefaultObject())->GetComponentsByClass(UHitBoxIDComp::StaticClass())[this->m_state]));
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
		if (ChildColliderActorRef)
		{
			ChildColliderActorRef->DestroyChildActor();
			return;
		}
	}

	if (ChildColliderActorRef)
	{
		if (ChildColliderActorRef->GetChildActor()) {

			DeOrActivateComponents(Cast<UHitBoxIDComp>(ChildColliderActorRef->GetChildActor()->GetComponentsByClass(UHitBoxIDComp::StaticClass())[this->m_state]));
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
