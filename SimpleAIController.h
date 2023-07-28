// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SimpleAIController.generated.h"

/**
 * 
 */
UCLASS()
class FLOPPOCODE_API ASimpleAIController : public AAIController
{
	GENERATED_BODY()


		UPROPERTY(EditDefaultsOnly, Category = AI)
			FName LocationFinderKey;

		UPROPERTY(EditDefaultsOnly, Category = AI)
			FName PlayerKey;
	
		 void OnPossess(APawn* SimplePawn) override; 
		
		class UBehaviorTreeComponent*	BehaviorComp;
		class UBlackboardComponent*	BlackboardComp;
		
		TArray<AActor*> PatrolPoints;
		

	

	
	

public: 
	ASimpleAIController();
	int32 CurrentPatrolPoint = 0;
	/*void SetPlayerCaught(APawn* Pawn);*/ 

	FORCEINLINE UBlackboardComponent* GetBlackBoardComp() const { return BlackboardComp; }
	FORCEINLINE TArray<AActor*> GetPatrolPoints() const { return PatrolPoints;} 

	
};
