// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MyCharacterBase.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "WindTunnel.generated.h"

UCLASS()
class ZELDALIKEDEMO_API AWindTunnel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWindTunnel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	bool bTemporaryWT = false;

	UPROPERTY(EditAnywhere)
	AMyCharacterBase* PlayerRef;

	UPROPERTY(editAnywhere)
	UBoxComponent* Box;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);
};
