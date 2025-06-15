// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MyCharacterBase.h"
#include "Data/MyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/MyLayout.h"
#include "Debug/DebugHelper.h"
#include "DrawDebugHelpers.h"

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

	// Set player rotates toward the direction according to inputs
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

	// Initialize Stamina
	CurrentStamina = MaxStamina;

	// Create stamina UI
	if (LayoutClassRef)
	{
		LayoutRef = CreateWidget<UMyLayout>(GetWorld(), LayoutClassRef);
		if (LayoutRef)
		{
			// Trigger ConstructDeferred event
			LayoutRef->ConstructDeferred(this);
			LayoutRef->AddToViewport();
		}
	}
}

void AMyCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (CurrentMT == EMovementTypes::MM_EXHAUSTED)
	{
		StartRecoverStamina();
		return;
	}

	if (CurrentMT == EMovementTypes::MM_GLIDING)
	{
		// If on the fly, switch to walking
		LocomotionManager(EMovementTypes::MM_WALKING);
		return;
	}

	// Check previous status
	if (PreviousMT == EMovementTypes::MM_SPRINTING)
	{
		// Keep sprinting if the previous status is sprinting
		LocomotionManager(EMovementTypes::MM_SPRINTING);
	}
	else
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

		EIComp->BindAction(JumpGlideAction, ETriggerEvent::Completed, this, &AMyCharacterBase::JumpGlide_Completed);
		EIComp->BindAction(JumpGlideAction, ETriggerEvent::Started, this, &AMyCharacterBase::JumpGlide_Started);
	}
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

#pragma region Locomotion
void AMyCharacterBase::Sprint_Triggered(const FInputActionValue& val)
{
	// If velocity equals to 0 (inputs = 0), cancel sprinting (energy is exhausted)
	if (Velocity_X == 0 && Velocity_Y == 0 && CurrentMT == EMovementTypes::MM_SPRINTING)
	{
		LocomotionManager(EMovementTypes::MM_WALKING);
	}
}

void AMyCharacterBase::Sprint_Started(const FInputActionValue& val)
{
	if (CurrentMT == EMovementTypes::MM_WALKING || CurrentMT == EMovementTypes::MM_MAX)
	{
		LocomotionManager(EMovementTypes::MM_SPRINTING);
	}
}

void AMyCharacterBase::Sprint_Completed(const FInputActionValue& val)
{
	if (CurrentMT == EMovementTypes::MM_SPRINTING)
	{
		LocomotionManager(EMovementTypes::MM_WALKING);
	}
}

void AMyCharacterBase::JumpGlide_Started(const FInputActionValue& val)
{
	if(CurrentMT == EMovementTypes::MM_EXHAUSTED) return;
	if (GetCharacterMovement()->MovementMode != MOVE_Falling)
	{
		// Save previous status
		PreviousMT = CurrentMT;
		// Can jump
		Jump();
		LocomotionManager(EMovementTypes::MM_FALLING);
	}

	if (CurrentMT == EMovementTypes::MM_GLIDING)
	{
		// If while gliding, cancel gliding and change to falling
		Debug::PrintInfo("CancelGliding");
		LocomotionManager(EMovementTypes::MM_FALLING);
	}

	// Check the distance between the ground and the player, cannot glide if too close to the ground
	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = Start - EnableGlideDistance;
	// Ignore the player itself
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool HitAnything = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.0f, 0.0f, 3.0f);

	if (HitAnything)
	{
		// If hit something, cannot glide
		Debug::PrintInfo("HitSomething");
	}
	else
	{
		Debug::PrintInfo("Not HitSomething");
		// If not hit anything, can glide
		// TODO: 取消激活释放技能

		// Switch to glide
		LocomotionManager(EMovementTypes::MM_GLIDING);
	}
}

void AMyCharacterBase::JumpGlide_Completed(const FInputActionValue& val)
{
	StopJumping();
}


void AMyCharacterBase::LocomotionManager(EMovementTypes NewMovement)
{
	// Control movement
	if (NewMovement == CurrentMT) return;

	CurrentMT = NewMovement;

	if (CurrentMT == EMovementTypes::MM_GLIDING)
	{
		// Show the glider model
	}

	switch (CurrentMT)
	{
	case EMovementTypes::MM_MAX:
		break;
	case EMovementTypes::MM_WALKING:
		SetWalking();
		break;
	case EMovementTypes::MM_EXHAUSTED:
		SetExhausted();
		break;
	case EMovementTypes::MM_SPRINTING:
		SetSprint();
		break;
	case EMovementTypes::MM_GLIDING:
		SetGliding();
		break;
	case EMovementTypes::MM_FALLING:
		break;
	}
}

void AMyCharacterBase::ResetToWalk()
{
	// If gravity added, cancel it
	GetWorldTimerManager().ClearTimer(AddGravityForFlyingTimerHandle);
	// Reset to ground status
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AMyCharacterBase::SetSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 1500.0f;
	GetCharacterMovement()->AirControl = 0.35f;

	ResetToWalk();

	// Consumer energy
	StartDrainStamina();
}

void AMyCharacterBase::SetWalking()
{
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;

	ResetToWalk();

	// Recover energy
	StartRecoverStamina();
}

void AMyCharacterBase::SetGliding()
{
	GetCharacterMovement()->AirControl = 0.6f;

	// Set flying mode 
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	StartDrainStamina();

	// Set gravity simulation, execute each frame
	AddGravityForFlying();

	GetWorldTimerManager().SetTimer(AddGravityForFlyingTimerHandle, this, &AMyCharacterBase::AddGravityForFlying,
	                                GetWorld()->GetDeltaSeconds(), true);
}

bool AMyCharacterBase::IsCharacterExhausted() const
{
	return CurrentMT == EMovementTypes::MM_EXHAUSTED;
}
#pragma endregion Locomotion

#pragma region Stamina
void AMyCharacterBase::SetExhausted()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.0f; // Slow walking
	GetCharacterMovement()->AirControl = 0.35f;

	ClearDrainRecoverStaminaTimer();

	// Recover energy when on the ground
	if (GetCharacterMovement()->MovementMode == MOVE_Walking)
	{
		StartRecoverStamina();
	}
	// If falling, start recovering energy until landing 
	else if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		ResetToWalk();
	}
}

void AMyCharacterBase::DrainStaminaTimer()
{
	if (CurrentStamina <= 0.0f)
	{
		LocomotionManager(EMovementTypes::MM_EXHAUSTED);
	}
	else
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - StaminaDepletionAmount, 0.0f, MaxStamina);
	}
}

void AMyCharacterBase::StartDrainStamina()
{
	// Clear existed Timer
	ClearDrainRecoverStaminaTimer();

	GetWorldTimerManager().SetTimer(DrainStaminaTimerHandle, this, &AMyCharacterBase::DrainStaminaTimer,
	                                StaminaDepletionRate, true);

	// Show UI
	if (LayoutRef)
	{
		// Trigger ShowGaugeAnim event
		LayoutRef->ShowGaugeAnim(true);
	}
}

void AMyCharacterBase::RecoverStaminaTimer()
{
	if (CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina + StaminaDepletionAmount, 0.0f, MaxStamina);
	}
	else
	{
		ClearDrainRecoverStaminaTimer();
		LocomotionManager(EMovementTypes::MM_WALKING);

		// Hide UI
		if (LayoutRef)
		{
			// Trigger ShowGaugeAnim event
			LayoutRef->ShowGaugeAnim(false);
		}
	}
}

void AMyCharacterBase::StartRecoverStamina()
{
	// Clear existed Timer
	ClearDrainRecoverStaminaTimer();

	GetWorldTimerManager().SetTimer(RecoverStaminaTimerHandle, this, &AMyCharacterBase::RecoverStaminaTimer,
	                                StaminaDepletionRate, true);
}

void AMyCharacterBase::ClearDrainRecoverStaminaTimer()
{
	GetWorldTimerManager().ClearTimer(DrainStaminaTimerHandle);
	GetWorldTimerManager().ClearTimer(RecoverStaminaTimerHandle);
}

void AMyCharacterBase::AddGravityForFlying()
{
	LaunchCharacter(FVector(0.0f, 0.0f, -100.0f), false, true);
}
#pragma endregion Stamina
