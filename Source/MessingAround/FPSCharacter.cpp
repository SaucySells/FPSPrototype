// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"

#define COLLISION_TRACE_GRAPPLEPOINT ECC_GameTraceChannel2
#define COLLISION_TRACE_VISIBILITY ECC_Visibility

const float GRAPPLE_ACCELERATION_BASE = .1;

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a first person camera component.
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FPSCameraComponent != nullptr);

	// Attach the camera component to our capsule component.
	FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));

	// Position the camera slightly above the eyes.
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));

	// Enable the pawn to control camera rotation.
	FPSCameraComponent->bUsePawnControlRotation = true;
	
	//The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);

	// Create a first person mesh component for the owning player.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FPSMesh != nullptr);

	// Only the owning player sees this mesh.
	FPSMesh->SetOnlyOwnerSee(true);

	// Attach the FPS mesh to the FPS camera.
	FPSMesh->SetupAttachment(FPSCameraComponent);

	// Disable some environmental shadows to preserve the illusion of having a single mesh.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;
	
	//GrappleHeld = false;
	IsGrappleSwinging = false;
	GrappleAcceleration = GRAPPLE_ACCELERATION_BASE;
	GrappleSwingDirection = 1;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Grapple Checking
	CheckForGrapplePoint();
	if (CurrentGrapple && GrappleHeld) 
	{
		Grapple(CurrentGrapple, DeltaTime);
		IsGrappling = true;
	}
	else 
	{
		IsGrappling = false;
		GetCharacterMovement()->GravityScale = 1.7;
		SetActorEnableCollision(true);
		GrappleAcceleration = GRAPPLE_ACCELERATION_BASE;
	}
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up movement bindings
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

	// Set up "action" bindings.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
	PlayerInputComponent->BindAction("Grapple", IE_Pressed, this, &AFPSCharacter::GrappleStart);
	PlayerInputComponent->BindAction("Grapple", IE_Released, this, &AFPSCharacter::GrappleEnd);
}

void AFPSCharacter::MoveForward(float value) 
{
	if (!MovementDisabled)
	{
		// Find out which way is "forward" and record that the player wants to move that way.
		FVector Direction = GetActorForwardVector();
		AddMovementInput(Direction, value);
	}
}

void AFPSCharacter::MoveRight(float value)
{
	if (!MovementDisabled)
	{
		// Find out which way is "right" and record that the player wants to move that way.
		FVector Direction = GetActorRightVector();
		AddMovementInput(Direction, value);
	}
}

void AFPSCharacter::StartJump() 
{
	bPressedJump = true;
}

void AFPSCharacter::StopJump()
{
	bPressedJump = false;
}

void AFPSCharacter::Fire()
{
	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		// Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 2.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
}

AGrapplePoint* AFPSCharacter::CheckForGrapplePoint() 
{
	// Check to see if the player is looking at a grapple point.
	UWorld* World = GetWorld();
	if (World)
	{
		FHitResult Hit;
		FHitResult VisibilityCheck;

		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 75.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		FVector EndLocation = MuzzleLocation + (CameraRotation.Vector() * GrappleRange);

		World->LineTraceSingleByChannel(Hit, CameraLocation, EndLocation, COLLISION_TRACE_GRAPPLEPOINT);
		World->LineTraceSingleByChannel(VisibilityCheck, CameraLocation, EndLocation, COLLISION_TRACE_VISIBILITY);
		
		// Grapple point is blocked by something in player's vision
		if (VisibilityCheck.bBlockingHit && (Hit.Distance > VisibilityCheck.Distance))
		{
			return nullptr;
		}

		AGrapplePoint* GrapplePoint = Cast<AGrapplePoint>(Hit.GetActor());

		if (GrapplePoint)
		{
			GrapplePoint->DisplayIcon();
			return GrapplePoint;
		}

	}

	return nullptr;
}

void AFPSCharacter::Grapple(AGrapplePoint* GrapplePoint, float DeltaTime)
{
	if (GrapplePoint)
	{

		GetWorld()->LineBatcher->DrawLine(GetActorLocation(), GrapplePoint->GetActorLocation(),FLinearColor(0.0, 0.0, 1.0, 1.0),1, 3.0f, 1.0f);

		
		GetCharacterMovement()->GravityScale = 0;
		SetActorEnableCollision(false);
		FVector BottomOfSwingArc = FVector(GrapplePoint->GetActorLocation().X, GrapplePoint->GetActorLocation().Y, GrapplePoint->GetActorLocation().Z - 900.0f);

		float DistanceToGrapplePointBottomSquared = (FVector::DistSquared(GetActorLocation(), BottomOfSwingArc));
		if (IsGrappleSwinging) 
		{
			if (GrappleSwingAngleAxis <= 100.0f)
			{
				GrappleSwingDirection = 1;
			}
			else if (GrappleSwingAngleAxis >= 260.0f)
			{
				GrappleSwingDirection = -1;
			}

			FVector NewLocation = GrapplePoint->GetActorLocation();

			// -.00021(x - 180)^2 + 1.5, where x is the current angle axis, speeds up the player in the middle of the swing and slows them down at each end of it
			GrappleSwingAngleAxis += (DeltaTime * GrappleSwingMultiplier * (-.00021*((GrappleSwingAngleAxis - 180)*(GrappleSwingAngleAxis - 180)) + 1.5 )) * GrappleSwingDirection;

			FVector RotateValue = GrappleSwingDimensions.RotateAngleAxis(GrappleSwingAngleAxis, GrappleSwingAxisVector);


			NewLocation.X += RotateValue.X;
			NewLocation.Y += RotateValue.Y;
			NewLocation.Z += RotateValue.Z;

			SetActorLocation(NewLocation);

		}
		else if (DistanceToGrapplePointBottomSquared >= 7500.0f)
		{
			FVector PullDirection = BottomOfSwingArc - GrappleStartLocation;
			float PullMagnitude = FMath::Clamp(PullDirection.Size() * (DeltaTime * GrappleAcceleration), 4.0f, 30.0f);

			PullDirection.Normalize();

			SetActorLocation(GetActorLocation() + (PullDirection * PullMagnitude));

			GrappleAcceleration += 0.06;

			// Check if next frame will be within range to swing.
			if (FVector::DistSquared(GetActorLocation() + (PullDirection * PullMagnitude), BottomOfSwingArc) < 7500.0f)
			{
				IsGrappleSwinging = true;
			}
			else 
			{
				IsGrappleSwinging = false;
			}
		}
		// Player is inside grapple radius
		else 
		{
			IsGrappleSwinging = true;
		}
	}
}

void AFPSCharacter::CameraLook(FVector TargetLocation)
{
	FVector DirectionVector = FPSCameraComponent->GetComponentLocation() - TargetLocation;
	DirectionVector.Normalize();
	FaceRotation(FRotator(DirectionVector.ToOrientationQuat()));
}

void AFPSCharacter::GrappleStart()
{
	CurrentGrapple = CheckForGrapplePoint();
	if (CurrentGrapple) {
		FVector rotation = GetActorRotation().Vector();
		GrappleSwingAxisVector = FVector(rotation.Y, rotation.X*-1, 0);

		//FVector PlayerToGrappleLine = (GetActorLocation() - CurrentGrapple->GetActorLocation());
		//PlayerToGrappleLine.Normalize();

		//float angle = (acos(FVector::DotProduct(PlayerToGrappleLine, FVector(PlayerToGrappleLine.X, PlayerToGrappleLine.Y, 0.0f))) * 180)/PI;

		GrappleSwingAngleAxis = 180.0f;
		/*if (GrappleSwingAngleAxis < 100) 
		{
			GrappleSwingAngleAxis = 99;
		}*/
		GrappleSwingDirection = 1;

		GrappleStartLocation = GetActorLocation();
		GetCharacterMovement()->Velocity = FVector(0.0f, 0.0f, 0.0f);
	}
	GrappleHeld = true;
}

void AFPSCharacter::GrappleEnd()
{
	GrappleHeld = false;
	CurrentGrapple = nullptr;
	if (IsGrappleSwinging)
	{
		FVector rotation = GetActorRotation().Vector();
		GetCharacterMovement()->Velocity = GrappleEjectVelocity * FVector(rotation.X, rotation.Y, 0.6f);
		GetCharacterMovement()->UpdateComponentVelocity();
	}
	IsGrappleSwinging = false;
}
