// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplePoint.h"

// Sets default values
AGrapplePoint::AGrapplePoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	genericRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	RootComponent = genericRootComponent;

	//GrappleableIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("GrappleableIcon"));
	//check(GrappleableIcon != nullptr);
}

// Called when the game starts or when spawned
void AGrapplePoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrapplePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// TODO: Find a better way to access the grapple icon billboard
	RootComponent->GetChildComponent(0)->SetVisibility(false);

}

// Display the icon to show that the player can use this grapple point
void AGrapplePoint::DisplayIcon()
{
	// TODO: Find a better way to access the grapple icon billboard
	RootComponent->GetChildComponent(0)->SetVisibility(true);
	return;
}
