// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/LineBatchComponent.h"
#include "FPSProjectile.h"
#include "GrapplePoint.h"

#include "FPSCharacter.generated.h"

UCLASS()
class MESSINGAROUND_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFPSProjectile> ProjectileClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	AGrapplePoint* CheckForGrapplePoint();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles input for moving forward and backward.
	UFUNCTION()
	void MoveForward(float value);

	// Handles input for moving right and left.
	UFUNCTION()
	void MoveRight(float value);

	// Sets jump flag when key is pressed.
	UFUNCTION()
	void StartJump();

	// Sets jump flag when key is released.
	UFUNCTION()
	void StopJump();

	// Function that handles firing projectiles.
	UFUNCTION()
	void Fire();

	UFUNCTION()
	void Grapple(AGrapplePoint* GrapplePoint, float DeltaTime);

	void GrappleStart();
	void GrappleEnd();

	// Sets the camera to look at an location
	void CameraLook(FVector TargetLocation);

	// FPS camera.
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FPSCameraComponent;

	// First-person mesh (arms), visible only to the owning player.
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FPSMesh;

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	int GrappleRange;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	bool MovementDisabled;

	AGrapplePoint* CurrentGrapple;
	bool GrappleHeld;
	float GrappleAcceleration;
	bool IsGrappleSwinging;
	int GrappleSwingDirection;
	FVector GrappleStartLocation;

	UPROPERTY(EditAnywhere, Category = Movement)
	float GrappleEjectVelocity;

	UPROPERTY(EditAnywhere, Category = Movement)
	bool IsGrappling;

	float GrappleSwingAngleAxis;

	UPROPERTY(EditAnywhere, Category = Movement)
	FVector GrappleSwingDimensions;

	UPROPERTY(EditAnywhere, Category = Movement)
	FVector GrappleSwingAxisVector;

	UPROPERTY(EditAnywhere, Category = Movement)
	float GrappleSwingMultiplier;

};
