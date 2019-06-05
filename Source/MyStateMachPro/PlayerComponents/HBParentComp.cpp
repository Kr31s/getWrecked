// Fill out your copyright notice in the Description page of Project Settings.


#include "HBParentComp.h"
#include "FGDefaultPawn.h"
#include "HitBoxIDComp.h"


// Sets default values for this component's properties
UHBParentComp::UHBParentComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...

}


// Called when the game starts
void UHBParentComp::BeginPlay()
{
	Super::BeginPlay();
	player = Cast<AFGDefaultPawn>(GetOwner());

	this->GetChildrenComponents(false, TIDChildren);
	
	for (int i = 0; i < 3; ++i)
	{

		Cast<UHitBoxIDComp>(TIDChildren[i])->MoveID = i;
	}
	this->Deactivate();

		
	
	// ...
	
}


// Called every frame
void UHBParentComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GEngine->AddOnScreenDebugMessage(-1, 1.0F, FColor::Red, TEXT("HBParent!"));

	// ...
}

void UHBParentComp::SwitchBetweenChildren()
{
	switch (player->Id)
	{
	case 0:
		TIDChildren[0]->SetActive(true);
		TIDChildren[1]->SetActive(false);
		TIDChildren[2]->SetActive(false);
		break;
	case 1:
		TIDChildren[0]->SetActive(false);
		TIDChildren[1]->SetActive(true);
		TIDChildren[2]->SetActive(false);
		break;

	case 2:
		TIDChildren[0]->SetActive(false);
		TIDChildren[1]->SetActive(false);
		TIDChildren[2]->SetActive(true);
		break;
	default:
		TIDChildren[0]->Deactivate();
		TIDChildren[1]->Deactivate();
		TIDChildren[2]->Deactivate();
		break;
	}
}
