// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WindTunnel.h"


// Sets default values
AWindTunnel::AWindTunnel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);

	// Bind callback event
	Box->OnComponentBeginOverlap.AddDynamic(this, &AWindTunnel::OnOverlapBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &AWindTunnel::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AWindTunnel::BeginPlay()
{
	Super::BeginPlay();

	if (bTemporaryWT)
	{
		InitialLifeSpan = 30.0f;
	}
}

// Called every frame
void AWindTunnel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!PlayerRef) return;

	if (PlayerRef->CurrentMT != EMovementTypes::MM_GLIDING) return;

	FVector LocalUpVec = GetActorUpVector() * 10.0f;

	PlayerRef->AddActorWorldOffset(LocalUpVec);
}

void AWindTunnel::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	PlayerRef = Cast<AMyCharacterBase>(OtherActor);
}

void AWindTunnel::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerRef = nullptr;
}
