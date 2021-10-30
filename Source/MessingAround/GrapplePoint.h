// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"

#include "GrapplePoint.generated.h"

UCLASS()
class MESSINGAROUND_API AGrapplePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrapplePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Display the icon to show that the player can use this grapple point
	void DisplayIcon();

	UPROPERTY(EditAnywhere, Category="Root")
	USceneComponent* genericRootComponent;

	UPROPERTY(EditAnywhere, Category="GrappleableIcon")
	UBillboardComponent* GrappleableIcon;
};
