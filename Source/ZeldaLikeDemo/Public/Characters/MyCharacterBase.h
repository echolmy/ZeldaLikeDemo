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

UENUM(BlueprintType)
enum class EMovementTypes : uint8
{
	MM_MAX UMETA(DisplayName = "Max"), // default
	MM_WALKING UMETA(DisplayName = "Walking"), // move on the ground
	MM_EXHAUSTED UMETA(DisplayName = "Exhausted"), // exhausted, cannot run, sprint and glide
	MM_SPRINTING UMETA(DisplayName = "Sprinting"), // sprinting
	MM_GLIDING UMETA(DisplayName = "GLiding"), // gliding
	MM_FALLING UMETA(DisplayName = "Falling"), // falling, cannot run and sprint
};

UCLASS()
class ZELDALIKEDEMO_API AMyCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacterBase();

	UPROPERTY(EditAnywhere, Category = "Comps")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, Category="Comps")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, Category="Inputs")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category="Inputs")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Inputs")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category="Inputs")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, Category="Movement")
	EMovementTypes CurrentMovementMode{EMovementTypes::MM_MAX};

	// Input value
	float Velocity_X;
	float Velocity_Y;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Inputs Node
	UFUNCTION()
	void Move_Triggered(const FInputActionValue& val);

	UFUNCTION()
	void Move_Completed(const FInputActionValue& val);

	UFUNCTION()
	void Look_Triggered(const FInputActionValue& val);

	UFUNCTION()
	void Sprint_Triggered(const FInputActionValue& val);

	UFUNCTION()
	void Sprint_Started(const FInputActionValue& val);

	UFUNCTION()
	void Sprint_Completed(const FInputActionValue& val);

#pragma endregion Inputs Node

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void LocomotionManager(EMovementTypes NewMovement);

	void ResetToWalk();
	
	void SetSprint();

	void SetWalking();
};
