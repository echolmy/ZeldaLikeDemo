// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/MyCharacterBase.h"
#include "MyLayout.generated.h"

/**
 * 
 */
class AMyCharacterBase;

UCLASS()
class ZELDALIKEDEMO_API UMyLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(blueprintimplementableEvent)
	void ConstructDeferred(AMyCharacterBase* PlayerRef);

	UFUNCTION(blueprintimplementableEvent)
	void ShowGaugeAnim(bool bShow);
};
