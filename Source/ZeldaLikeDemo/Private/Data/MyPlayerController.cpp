// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeldaLikeDemo/Public/Data/MyPlayerController.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
}
