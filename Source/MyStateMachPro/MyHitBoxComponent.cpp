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

	if (!IsTemplate()) // IsTemplate avoids calling this code on CDOs (Default Objects)
	{

	}
	// PostInitProperties() --> start straight after constructor (CDO?)
}

void UMyHitBoxComponent::PostInitProperties()
{

	Super::PostInitProperties();
	this->SetCollisionResponseToChannel(ECC_Visibility, ::ECR_Overlap);
	this->SetCollisionResponseToChannel(ECC_Camera, ::ECR_Overlap);
	this->SetCollisionResponseToChannel(ECC_WorldDynamic, ::ECR_Overlap);
	if (!IsTemplate()) //
	{
	}

	this->OnComponentBeginOverlap.AddDynamic(this, &UMyHitBoxComponent::CollisionEvent);
	this->OnComponentEndOverlap.AddDynamic(this, &UMyHitBoxComponent::CollisionEndEvent);
	TArray<UPrimitiveComponent*> overlaps;
	GetOverlappingComponents(overlaps);
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

#pragma optimize("", off)
void UMyHitBoxComponent::CollisionEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto* targetCollider = Cast<UMyHitBoxComponent>(OtherComp);
	if (Cast<AFGDefaultPawn>(OverlappedComponent->GetOwner()->GetAttachParentActor()))
	{
		Owner = Cast<AFGDefaultPawn>(OverlappedComponent->GetOwner()->GetAttachParentActor());

		//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Red, Owner->GetName());

	}
	else
	{

		//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Red, Owner->GetName());
	}
	//
	if (targetCollider) // Check for Valid Cast to UMyHitBoxComponent
	{
		Enemy = Cast<AFGDefaultPawn>(targetCollider->GetOwner()->GetAttachParentActor());


		//Both player hitting at the same time --> no damage apply
		if (targetCollider->Etype == EBoxType::Hit)
		{
			if (Enemy && Owner) // valid check
			{
				if (Owner->Opponent == Enemy) // check if atk collider owner opponent is, targeted collider owner
				{
					if (this->Etype == EBoxType::Hit)
					{
						Enemy->gotHit = true;
						Enemy->RessourceComp->ReduceHealth(0.0F);
						const FVector EmitterSpawnLocation2 = OverlappedComponent->GetComponentLocation();
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Owner->gotHitFire, FVector(EmitterSpawnLocation2.X, 0, EmitterSpawnLocation2.Z), FRotator(0.0f, 0.0f, 0.0f), FVector(0.3F, 0.3F, 0.3F), true);
						//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("Both player hitting each other at the same time"));
						return;
					}
				}
			}

		}

		if (targetCollider->Etype == EBoxType::Hurt)
		{
			if (Enemy && Owner) // valid check
			{
				if (Owner->Opponent == Enemy) // check if atk collider owner opponent is, targeted collider owner
				{


					switch (this->Etype)
					{
					case EBoxType::Hit: // Damage Collider, Place to Apply Damage On the Enemy if isnt Blocking

						if (!Enemy->bIsBlocking && Owner->canApplyDamage)
						{
							Enemy->gotHit = true;
							Enemy->RessourceComp->ReduceHealth(Owner->GetCurrentMove()->DamageValue);
							Enemy->RessourceComp->IncreaseStunMeter(0.05F);
							//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Red, TEXT("HitBoxCollision"));

							const FVector EmitterSpawnLocation2 = OverlappedComponent->GetComponentLocation();
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Owner->gotHitFire, FVector(EmitterSpawnLocation2.X, 0, EmitterSpawnLocation2.Z), FRotator(0.0f, 0.0f, 0.0f), FVector(0.3F, 0.3F, 0.3F), true);
							Owner->canApplyDamage = false;
						}
						else
						{
							//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("EnemyIsBlocking Or Just Received Damage"));

						}
						if (Enemy->bIsBlocking)
						{
							Enemy->gotHit = true;
						}

						break;
					case EBoxType::Block:
						// FIX COLLIDING WITH SELF !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						Enemy->SetCanBlock(true);

						//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("BlockBoxCollision"));

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
				if (Owner->Opponent == Enemy) // check if atk collider owner opponent is, targeted collider owner
				{

					switch (Etype)
					{
					case EBoxType::Block:
							Enemy->SetCanBlock(false);
							//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("EndOverlapCollision"));
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
}
#pragma optimize("", on)
