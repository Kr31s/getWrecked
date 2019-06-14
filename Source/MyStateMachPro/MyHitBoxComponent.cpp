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
	//owner = Cast<AFGDefaultPawn>(this->GetOwner());

//	auto* collider{ Cast<UMyHitBoxComponent>(OtherComp) };
//	if(collider)
//	{
//			//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("CastSUCCESS"));
//			auto* Opponent = Cast<AFGDefaultPawn>(collider->GetOwner());
////			opponent->RessourceComp->ReduceHealth(0.05F/*Cast<AFGDefaultPawn>(this->GetOwner())->GetCurrentMove()->DamageValue*/);
//	}else
//	{
//			//GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("CastFAILED"));
//	}
	//MyOwner = GetOwner()->GetAttachParentActor();

	 
	switch (Etype)
	{
	case EBoxType::Hit:
		

		if (OtherComp->GetName() == TEXT("CollisionCylinder") )//&& MyOwner != OtherComp->GetOwner())//(collider->Etype == EBoxType::Hurt)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Red, TEXT("HitBoxCollision"));

		}
		break;
	case EBoxType::Block:
		if (OtherComp->GetName() == TEXT("CollisionCylinder"))//(collider->Etype == EBoxType::Hurt)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, OtherComp->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 2.0F, FColor::Blue, TEXT("BlockBoxCollision"));
		}
		break;
	case EBoxType::Hurt:
		break;
	default:
		break;
	}
	 
	 



}

