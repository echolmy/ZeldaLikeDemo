// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyCharacterBase.generated.h"

class UInputAction;
class UInputMappingContext;
class UMyLayout;

/**
 * Enumeration defining different movement types for the character.
 * Used to control character locomotion states and related behaviors.
 */
UENUM(BlueprintType)
enum class EMovementTypes : uint8
{
	MM_MAX UMETA(DisplayName = "Max"), // default
	MM_WALKING UMETA(DisplayName = "Walking"), // move on the ground
	MM_EXHAUSTED UMETA(DisplayName = "Exhausted"), // exhausted, cannot run, sprint and glide
	MM_SPRINTING UMETA(DisplayName = "Sprinting"), // sprinting
	MM_GLIDING UMETA(DisplayName = "GLiding"), // gliding
	MM_FALLING UMETA(DisplayName = "Falling"), // falling cannot run and sprint
};

UENUM(BlueprintType)
enum class ERunes : uint8
{
	R_EMAX UMETA(DisplayName = "EMax"), // Default
	R_RBS UMETA(DisplayName = "RBS"), // Remote bomb sphere
	R_RBB UMETA(DisplayName = "RBB"), // Remote bomb box
	R_MAG UMETA(DisplayName = "MAGNET"), // Magnet
	R_STAT UMETA(DisplayName = "STATICS"), // Time stop
	R_ICE UMETA(DisplayName = "ICE"), // Generate ice
};

/**
 * Base character class that implements movement, camera control, and stamina systems.
 * Provides functionality for walking, sprinting, and handling exhaustion states.
 * Integrates with an Enhanced Input system for modern input handling.
 */
UCLASS()
class ZELDALIKEDEMO_API AMyCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	/**
	 * Constructor for AMyCharacterBase.
	 * Sets default values for this character's properties including movement, camera setup, and rotation settings.
	 */
	AMyCharacterBase();

	/** Spring arm component that positions the camera behind the character */
	UPROPERTY(EditAnywhere, Category = "Comps")
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Camera component that provides the player's view */
	UPROPERTY(EditAnywhere, Category="Comps")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, Category="Comps")
	TObjectPtr<USkeletalMeshComponent> Parachute;

	/** Input mapping context for the character's input actions */
	UPROPERTY(EditAnywhere, Category="Inputs")
	TObjectPtr<UInputMappingContext> MappingContext;

	/** Input action for character movement */
	UPROPERTY(EditAnywhere, Category="Inputs")
	TObjectPtr<UInputAction> MoveAction;

	/** Input action for camera control */
	UPROPERTY(EditAnywhere, Category="Inputs")
	TObjectPtr<UInputAction> LookAction;

	/** Input action for sprint control */
	UPROPERTY(EditAnywhere, Category="Inputs")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, Category="Inputs")
	TObjectPtr<UInputAction> JumpGlideAction;

	/** Current movement type/state of the character */
	UPROPERTY(EditAnywhere, Category="Movement")
	EMovementTypes CurrentMT{EMovementTypes::MM_MAX};

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector EnableGlideDistance{0.0f, 0.0f, 150.0f};

	UPROPERTY()
	EMovementTypes PreviousMT;

	/** Class reference for the UI layout widget */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LayoutClassRef;

	/** Instance of the UI layout widget */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UMyLayout* LayoutRef;

	/** Horizontal movement input value */
	float Velocity_X;

	/** Vertical movement input value */
	float Velocity_Y;

	UPROPERTY(visibleanywhere, Category = "Runes")
	bool bReadyToThrow = false;

	UPROPERTY(EditAnywhere, Category = "Runes")
	ERunes ActiveRune{ERunes::R_EMAX};

protected:
	/**
	 * Called when the game starts or when spawned.
	 * Initializes input subsystems, stamina, and UI elements.
	 */
	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& Hit) override;

#pragma region Inputs Node
	/**
	 * Handles continuous movement input.
	 * @param val - The input action value containing movement direction data
	 */
	UFUNCTION()
	void Move_Triggered(const FInputActionValue& val);

	/**
	 * Handles movement input completion.
	 * Resets velocity values when movement input stops.
	 * @param val - The input action value
	 */
	UFUNCTION()
	void Move_Completed(const FInputActionValue& val);

	/**
	 * Handles camera look input.
	 * @param val - The input action value containing look direction data
	 */
	UFUNCTION()
	void Look_Triggered(const FInputActionValue& val);

#pragma endregion Inputs Node

#pragma region Sprint
	/**
	 * Handles continuous sprint input.
	 * Manages sprint cancellation when velocity is zero.
	 * @param val - The input action value
	 */
	UFUNCTION()
	void Sprint_Triggered(const FInputActionValue& val);

	/**
	 * Handles sprint input start.
	 * Initiates sprinting movement mode.
	 * @param val - The input action value
	 */
	UFUNCTION()
	void Sprint_Started(const FInputActionValue& val);

	/**
	 * Handles sprint input completion.
	 * Returns to walking movement mode.
	 * @param val - The input action value
	 */
	UFUNCTION()
	void Sprint_Completed(const FInputActionValue& val);
#pragma endregion Sprint

#pragma region Jump & Glide
	UFUNCTION()
	void JumpGlide_Started(const FInputActionValue& val);

	UFUNCTION()
	void JumpGlide_Completed(const FInputActionValue& val);
#pragma endregion Jump & Glide
	
public:
	/**
	 * Called every frame.
	 * @param DeltaTime - The time elapsed since the last frame
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Called to bind functionality to input.
	 * Sets up Enhanced Input actions for movement, camera control, and sprinting.
	 * @param PlayerInputComponent - The input component to bind to
	 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/**
	 * Manages transitions between different movement types.
	 * Controls character behavior based on the new movement state.
	 * @param NewMovement - The movement type to transition to
	 */
	UFUNCTION()
	void LocomotionManager(EMovementTypes NewMovement);

	/**
	 * Resets character to walking movement mode.
	 * Restores ground-based movement parameters.
	 */
	void ResetToWalk();

	/**
	 * Configures character for sprinting movement.
	 * Increases speed and begins stamina depletion.
	 */
	void SetSprint();

	/**
	 * Configures character for walking movement.
	 * Sets normal speed and begins stamina recovery.
	 */
	void SetWalking();

	void SetGliding();

	void SetFalling();


#pragma region Stamina
	/** Current stamina value */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stamina")
	float CurrentStamina = 0.0f;

	/** Maximum possible stamina value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
	float MaxStamina = 100.0f;

	/** How frequently stamina updates (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
	float StaminaDepletionRate = 0.05f;

	/** Amount of stamina depleted/recovered per update */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina")
	float StaminaDepletionAmount = 0.5f;

	/** Timer handle for stamina depletion */
	FTimerHandle DrainStaminaTimerHandle;

	/**
	* Checks if the character is in an exhausted state.
	* @return true if the character is exhausted, false otherwise
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCharacterExhausted() const;

	void SetExhausted();

	/**
	 * Periodically reduces stamina during sprinting.
	 * Transitions to exhausted state when stamina is depleted.
	 */
	void DrainStaminaTimer();

	/**
	 * Begins the stamina depletion process.
	 * Sets up the timer for regular stamina reduction and shows UI gauge.
	 */
	void StartDrainStamina();

	/** Timer handle for stamina recovery */
	FTimerHandle RecoverStaminaTimerHandle;

	/**
	 * Periodically increases stamina during walking.
	 * Hides stamina UI when fully recovered.
	 */
	void RecoverStaminaTimer();

	/**
	 * Begins the stamina recovery process.
	 * Sets up timer for regular stamina increase.
	 */
	void StartRecoverStamina();

	/**
	 * Clears all stamina-related timers.
	 * Used when transitioning between movement states.
	 */
	void ClearDrainRecoverStaminaTimer();

	FTimerHandle AddGravityForFlyingTimerHandle;

	void AddGravityForFlying();

#pragma endregion Stamina
};
