// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RuneSelection.h"

void URuneSelection::SelectRuneTypes(ERunes RuneType)
{
	if (!PlayerRef) return;
	PlayerRef->ActiveRune = RuneType;
}
