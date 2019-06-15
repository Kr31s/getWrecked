// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoFrameActor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ADemoFrameActor::ADemoFrameActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	id = 1;

	//Mesh need to be Attached to Root 
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComponent;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ADemoFrameActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADemoFrameActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ADemoFrameActor::DisableComponent(TArray<UPrimitiveComponent*> Box)
{
	for (UPrimitiveComponent* boxComp : Box)
	{
		
		boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
}


