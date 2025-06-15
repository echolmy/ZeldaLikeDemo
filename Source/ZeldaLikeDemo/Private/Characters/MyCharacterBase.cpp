// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MyCharacterBase.h"
#include "Data/MyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Debug/DebugHelper.h"

// Sets default values
AMyCharacterBase::AMyCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Set Camera Boom and Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	// Set player rotate toward the direction according to inputs
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
}

// Called when the game starts or when spawned
void AMyCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	TObjectPtr<AMyPlayerController> PC = Cast<AMyPlayerController>(GetController());
	if (!PC) return;

	// Get Enhanced Input Subsystem for each local player
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem) return;

	Subsystem->AddMappingContext(MappingContext, 0);
}

#pragma region Move & Camera
void AMyCharacterBase::Move_Triggered(const FInputActionValue& val)
{
	const FVector2D InputVector = val.Get<FVector2D>();
	Velocity_X = InputVector.X;
	Velocity_Y = InputVector.Y;

	if (!Controller) return;

	// Only focus on Yaw horizontally
	const FRotator GroundRotation(0, Controller->GetControlRotation().Yaw, 0);

	// Left & Right
	const FVector RightDir = FRotationMatrix(GroundRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDir, Velocity_X);

	// Forward & Backward
	const FVector ForwardDir = FRotationMatrix(GroundRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDir, Velocity_Y);
}

void AMyCharacterBase::Move_Completed(const FInputActionValue& val)
{
	Velocity_X = 0;
	Velocity_Y = 0;
}

void AMyCharacterBase::Look_Triggered(const FInputActionValue& val)
{
	const FVector2D LookVal = val.Get<FVector2D>();

	AddControllerYawInput(LookVal.X);
	AddControllerPitchInput(LookVal.Y);
}
#pragma endregion Move & Camera

void AMyCharacterBase::Sprint_Triggered(const FInputActionValue& val)
{
	// If velocity equals to 0 (inputs = 0), cancel sprinting (energy is exhausted)
	if (Velocity_X == 0 && Velocity_Y == 0 && CurrentMovementMode == EMovementTypes::MM_SPRINTING)
	{
		LocomotionManager(EMovementTypes::MM_WALKING);
	}
}

void AMyCharacterBase::Sprint_Started(const FInputActionValue& val)
{
	if (CurrentMovementMode == EMovementTypes::MM_WALKING || CurrentMovementMode == EMovementTypes::MM_MAX)
	{
		LocomotionManager(EMovementTypes::MM_SPRINTING);
	}
}

void AMyCharacterBase::Sprint_Completed(const FInputActionValue& val)
{
	if (CurrentMovementMode == EMovementTypes::MM_SPRINTING)
	{
		LocomotionManager(EMovementTypes::MM_WALKING);
	}
}


// Called every frame
void AMyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacterBase::Move_Triggered);
		EIComp->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyCharacterBase::Move_Completed);

		EIComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacterBase::Look_Triggered);

		EIComp->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMyCharacterBase::Sprint_Triggered);
		EIComp->BindAction(SprintAction, ETriggerEvent::Started, this, &AMyCharacterBase::Sprint_Started);
		EIComp->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyCharacterBase::Sprint_Completed);
	}
}

void AMyCharacterBase::LocomotionManager(EMovementTypes NewMovement)
{
	// Control movement
	if (NewMovement == CurrentMovementMode)
	{
		return;
	}
	CurrentMovementMode = NewMovement;

	if (CurrentMovementMode == EMovementTypes::MM_GLIDING)
	{
		// Show glider model
	}

	switch (CurrentMovementMode)
	{
	case EMovementTypes::MM_MAX:
		break;
	case EMovementTypes::MM_WALKING:
		SetWalking();
		break;
	case EMovementTypes::MM_EXHAUSTED:
		Debug::PrintInfo("Exhausted");
		break;
	case EMovementTypes::MM_SPRINTING:
		SetSprint();
		break;
	case EMovementTypes::MM_GLIDING:
		break;
	case EMovementTypes::MM_FALLING:
		break;
	}
}

void AMyCharacterBase::ResetToWalk()
{
	// Reset to ground status
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AMyCharacterBase::SetSprint()
{
	Debug::PrintInfo("Sprinting");
	GetCharacterMovement()->MaxWalkSpeed = 1500.0f;
	GetCharacterMovement()->AirControl = 0.35f;

	ResetToWalk();

	// Consumer energy
}

void AMyCharacterBase::SetWalking()
{
	Debug::PrintInfo("Walking");
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;

	ResetToWalk();

	// Recover energy
}
