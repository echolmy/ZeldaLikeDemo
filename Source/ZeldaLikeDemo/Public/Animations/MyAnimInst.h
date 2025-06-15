// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/MyCharacterBase.h"
#include "MyAnimInst.generated.h"

/**
 * 
 */
UCLASS()
class ZELDALIKEDEMO_API UMyAnimInst : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	AMyCharacterBase* PlayerRef;

	UPROPERTY()
	UCharacterMovementComponent* MoveComp;

	UPROPERTY(visibleanywhere, BlueprintReadOnly, Category = "References")
	float GroundSpeed = 0.0f;

	UPROPERTY(visibleanywhere, BlueprintReadOnly, Category = "References")
	float AirSpeed = 0.0f;

	UPROPERTY(visibleanywhere, BlueprintReadOnly, Category = "References")
	bool bShouldMove = false;

	UPROPERTY(visibleanywhere, BlueprintReadOnly, Category = "References")
	bool bIsFalling = false;

	UPROPERTY(visibleanywhere, BlueprintReadOnly, Category = "References")
	bool bIsGliding = false;

	UPROPERTY(visibleanywhere, BlueprintReadOnly, Category = "References")
	bool bReadyToThrow = false;
	
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
