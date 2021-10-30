// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/BoxComponent.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorIsOpen = false;
	DoorTimer = 0.0f;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DoorIsOpen)
	{
		DoorTimer += DeltaTime;
	}

	if (DoorTimer >= RemainOpenDuration)
	{
		CloseDoor();
	}
}

void ADoor::OpenDoor()
{
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	this->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	if (StaticMeshComponents.Num() >= 2)
	{
		// Element 0 is door's left side, element 1 is door's right side, when facing positive X/negative Y direction
		StaticMeshComponents[0]->SetRelativeLocation(FVector(0.0f, -200.0f, 140.0f));
		StaticMeshComponents[1]->SetRelativeLocation(FVector(0.0f, 200.0f, 140.0f));
	}

	TArray<UBoxComponent*> BoxComponent;
	this->GetComponents<UBoxComponent>(BoxComponent);

	if (BoxComponent.Num())
	{
		BoxComponent[0]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	DoorIsOpen = true;
}

void ADoor::CloseDoor()
{
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	this->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	if (StaticMeshComponents.Num() >= 2)
	{
		// Element 0 is door's left side, element 1 is door's right side, when facing positive X/negative Y direction
		StaticMeshComponents[0]->SetRelativeLocation(FVector(0.0f, -80.0f, 140.0f));
		StaticMeshComponents[1]->SetRelativeLocation(FVector(0.0f, 70.0f, 140.0f));
	}

	TArray<UBoxComponent*> BoxComponent;
	this->GetComponents<UBoxComponent>(BoxComponent);

	if (BoxComponent.Num())
	{
		BoxComponent[0]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	DoorTimer = 0.0f;
	DoorIsOpen = false;
}


