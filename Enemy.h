// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIStates.h"
#include "GameFramework/Character.h"

#include "Enemy.generated.h"

UCLASS()
class FLOPPOCODE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	EAIMovement::Type motionState; 
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	bool ChangeMovement(EAIMovement::Type newAIMovement);

	FVector PatrolPoints;
	void FindPath(FVector Points); 
	 
	void Dead();
	//bool isdead;
	float health;

	//void OnPlayerCaught(APawn * Pawn);

	UPROPERTY(EditAnywhere, Category = AI)
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = AI)
		class UPawnSensingComponent* PawnSensingComp;

};
