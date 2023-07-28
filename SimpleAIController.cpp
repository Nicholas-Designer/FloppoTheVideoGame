// Fill out your copyright notice in the Description page of Project Settings.

#include "SimpleAIController.h"
#include "AIEnemy.h"
#include "PathPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"




ASimpleAIController::ASimpleAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	PlayerKey = "Target";
	LocationFinderKey = "LocationToGo";

	CurrentPatrolPoint = 0;
}

//void ASimpleAIController::SetPlayerCaught(APawn * Pawn)
//{
//
//	if (BlackboardComp)
//	{
//		BlackboardComp->SetValueAsObject(PlayerKey, Pawn); 
//	}
//}

void ASimpleAIController::OnPossess(APawn * SimplePawn)
{

	Super::Possess(SimplePawn);
	AAIEnemy* AICharacter = Cast<AAIEnemy>(SimplePawn);
	
	if (AICharacter)
	{
		if (AICharacter->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*(AICharacter->BehaviorTree->BlackboardAsset));
		}

		 UGameplayStatics::GetAllActorsOfClass(GetWorld(), APathPoint::StaticClass(), PatrolPoints);


		BehaviorComp->StartTree(*AICharacter->BehaviorTree);

		
	}

}
 

