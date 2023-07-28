// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "SimpleAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AEnemy::AEnemy()
{
	/*PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSeningComp"));
	PawnSensingComp->SetPeripheralVisionAngle(90.0f);*/
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	health = 100;
	/*if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemy::OnPlayerCaught);
	}*/
	
	
}


// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AEnemy::ChangeMovement(EAIMovement::Type newAIMovement)
{
	EAIMovement::Type oldMovement = motionState;

	switch (newAIMovement)
	{
	case EAIMovement::PATROL:
		
		break;

	case EAIMovement::ATTACKING:

		break;

	case EAIMovement::DEAD :
		Dead();
		break;
	}

	return oldMovement != motionState;
}

void AEnemy::Dead()
{
	
		//Destroy(this);
		//SetActorEnableCollision(false);
	
}

//void AEnemy::OnPlayerCaught(APawn * Pawn)
//{
//	ASimpleAIController* AIController = Cast<ASimpleAIController>(GetController());
//
//
//	if (AIController)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("You Have Been Caught!"));
//		AIController->SetPlayerCaught(Pawn);
//	}
//
//
//}

void AEnemy::FindPath(FVector Points)
{
	// create a function to find the Points
}

