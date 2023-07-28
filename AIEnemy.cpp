// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemy.h"
#include "SimpleAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AAIEnemy::AAIEnemy()
{
 	
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSeningComp"));
	PawnSensingComp->SetPeripheralVisionAngle(90.0f);
}

// Called when the game starts or when spawned
void AAIEnemy::BeginPlay()
{
	Super::BeginPlay();
	

	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AAIEnemy::OnPlayerCaught);
	}
}



// Called to bind functionality to input
void AAIEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



void AAIEnemy::OnPlayerCaught(APawn * Pawn)
{
	ASimpleAIController* AIController = Cast<ASimpleAIController>(GetController());


	if (AIController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("You Have Been Caught!"));
		//AIController->SetPlayerCaught(Pawn);
	}


}



