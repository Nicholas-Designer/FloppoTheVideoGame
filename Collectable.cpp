// Fill out your copyright notice in the Description page of Project Settings.


#include "Collectable.h"

// Sets default values
ACollectable::ACollectable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root component"));
	RootComponent = _RootComponent;


	CollectableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collectable Mesh"));
	CollectableMesh->SetupAttachment(RootComponent);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction widget"));
	InteractionWidget->SetupAttachment(RootComponent);
	

}

// Called when the game starts or when spawned
void ACollectable::BeginPlay()
{
	Super::BeginPlay();
	InteractionWidget->SetVisibility(false);
	
	
}

// Called every frame
void ACollectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACollectable::InteractWithMe()
{
	
		UE_LOG(LogTemp, Warning, TEXT("You Have Interacted with me!"));
		this->Destroy();
	
	

	

}

void ACollectable::ShowInteractionWidget()
{
	InteractionWidget->SetVisibility(true);
}

void ACollectable::HideInteractionWidget()
{
	InteractionWidget->SetVisibility(false);
}
