// Fill out your copyright notice in the Description page of Project Settings.


#include "HBParentComp.h"
#include "FGDefaultPawn.h"


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
	this->Deactivate();
	// ...
	
}


// Called every frame
void UHBParentComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(player->GetCurrentMove() == Move)
	{
		this->Activate();
	}else
	{
		this->Deactivate();
	}
	// ...
}

