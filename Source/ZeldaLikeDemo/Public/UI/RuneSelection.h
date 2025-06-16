// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/MyCharacterBase.h"
#include "RuneSelection.generated.h"

/**
 * 
 */
UCLASS()
class ZELDALIKEDEMO_API URuneSelection : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AMyCharacterBase* PlayerRef;

	UFUNCTION(blueprintcallable, Category = "RuneSelection")
	void SelectRuneTypes(ERunes RuneType);
};
