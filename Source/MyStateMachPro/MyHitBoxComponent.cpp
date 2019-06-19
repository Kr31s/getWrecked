// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHitBoxComponent.h"

UMyHitBoxComponent::UMyHitBoxComponent() {

	Etype = EBoxType::Hurt;
	this->ShapeColor = FColor::Green;
	this->SetCollisionProfileName("Custom");
	this->SetCollisionObjectType(ECC_WorldDynamic);
	this->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	this->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if(!IsTemplate()) // 
	{
		this->OnComponentBeginOverlap.AddDynamic(this, &UMyHitBoxComponent::CollisionEvent);
		this->OnComponentEndOverlap.AddDynamic(this, &UMyHitBoxComponent::CollisionEndEvent);
		TArray<UPrimitiveComponent*> overlaps;
		GetOverlappingComponents(overlaps);
	}
	// PostInitProperties() --> start straight after constructor (CDO?)
}

void UMyHitBoxComponent::PostInitProperties()
{

	Super::PostInitProperties();
	this->SetCollisionResponseToChannel(ECC_Visibility,::ECR_Overlap);
	this->SetCollisionResponseToChannel(ECC_Camera,::ECR_Overlap);
	this->SetCollisionResponseToChannel(ECC_WorldDynamic,::ECR_Overlap);
	if (!IsTemplate()) // 
	{
	}

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
	auto* targetCollider = Cast<UMyHitBoxComponent>(OtherComp);
	Owner = Cast<AFGDefaultPawn>(OverlappedComponent->GetOwner()->GetAttachParentActor());
	//
	if (targetCollider) // Check for Valid Cast to UMyHitBoxComponent
	{
		Enemy = Cast<AFGDefaultPawn>(targetCollider->GetOwner()->GetAttachParentActor());
		if (targetCollider->Etype == EBoxType::Hurt)
		{
			if (Enemy && Owner)
			{
				switch (Etype)
				{
				case EBoxType::Block:
					// FIX COLLIDING WITH SELF !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						Enemy->SetCanBlock(true);
						GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("BlockBoxCollision"));
					
					break;
				case EBoxType::Hit: // Damage Collider, Place to Apply Damage On the Enemy if isnt Blocking
					
						if(Owner->Opponent == Enemy && Owner->canApplyDamage)
						{
							if (!Enemy->bIsBlocking)
							{
								Enemy->gotHit = true;
								Enemy->RessourceComp->ReduceHealth(Owner->GetCurrentMove()->DamageValue);
								Enemy->RessourceComp->IncreaseStunMeter(0.05F);
								GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Red, TEXT("HitBoxCollision"));

								const FVector EmitterSpawnLocation2 = OverlappedComponent->GetComponentLocation();
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Owner->gotHitFire, FVector(EmitterSpawnLocation2.X, 0, EmitterSpawnLocation2.Z), FRotator(0.0f, 0.0f, 0.0f), FVector(0.3F, 0.3F, 0.3F), true);
							}else
							{
								GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("EnemyIsBlocking"));

							}
						}
					break;
				case EBoxType::Hurt:
					break;
				default:
					break;
				}
			}
		}
	}
}
void UMyHitBoxComponent::CollisionEndEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	auto* targetCollider = Cast<UMyHitBoxComponent>(OtherComp);
	Owner = Cast<AFGDefaultPawn>(OverlappedComponent->GetOwner()->GetAttachParentActor());
	//
	if (targetCollider)
	{
		Enemy = Cast<AFGDefaultPawn>(targetCollider->GetOwner()->GetAttachParentActor());
		if (targetCollider->Etype == EBoxType::Hurt)
		{

			if (Enemy && Owner)
			{
				switch (Etype)
				{
				case EBoxType::Block:
						Enemy->SetCanBlock(false);
						GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("EndOverlapCollision"));
					break;

				case EBoxType::Hurt:
					break;
				default:
					break;
				}

			}

		}
	}
}
