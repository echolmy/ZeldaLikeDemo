// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/MyAnimInst.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMyAnimInst::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerRef = Cast<AMyCharacterBase>(TryGetPawnOwner());
	if (!PlayerRef) return;
	MoveComp = PlayerRef->GetCharacterMovement();
}

void UMyAnimInst::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!PlayerRef || !MoveComp) return;

	GroundSpeed = UKismetMathLibrary::VSizeXY(PlayerRef->GetVelocity());
	AirSpeed = PlayerRef->GetVelocity().Z;
	bIsFalling = MoveComp->IsFalling();
	bShouldMove = !bIsFalling && GroundSpeed > 5.0f && MoveComp->GetCurrentAcceleration().Size() > 0;
	bIsGliding = PlayerRef->CurrentMT == EMovementTypes::MM_GLIDING;
	bReadyToThrow = PlayerRef->bReadyToThrow;
}
