// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MotionStates.h"
#include "Components/BoxComponent.h"
#include "InteractionInterface.h"
#include "FrogPawn.generated.h"





UCLASS()
class FLOPPOCODE_API AFrogPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFrogPawn();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditAnywhere)
		class USphereComponent* capsule;

	UPROPERTY(EditAnywhere)
		class UFloatingPawnMovement* pawnMovement;

	UPROPERTY(EditAnywhere)
		class UCameraComponent* camera;

	UPROPERTY(EditAnywhere)
		UBoxComponent* InteractionBox;


	UPROPERTY(EditAnywhere)
		class UUPlayerHUD* PlayerHUD;

	UPROPERTY(EditAnywhere)
		float Score;


	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUPlayerHUD> PlayerHUDClass;

	//player Information
	bool Died;

	//Maximum amount of health to allow for player
	UPROPERTY(EditAnywhere)
	float MaxHealth;
	// Current Health
	UPROPERTY(replicatedUsing = OnRepHealth)
	float Health;

	


	UFUNCTION()
	void HandleTakenAnyDamage(AActor* DamagedActor, float damage, const class UDamageType* DamageType, class AController* InstagatedBy, AActor* DamageCauser);

	UFUNCTION()
		void OnRepHealth();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	

	//JumpMovement
	float jumpSpeed;
	float jumpAcc;
	float jumpTimer;
	float jumpMax; 
	bool hasJumped; 
	void StartJump();
	void Jumping(float deltasecounds);
	void StopJump(bool sharp); 
	void Jump();
	void InterruptJump();
	bool IsInAir();
	void Grounded();
	UFUNCTION()
		void Ungrounded();

	FVector lookDirection;

	//Player movement
	FVector AddVol;
	FVector Vol;
	float walkSpeed;
	EMotionStates:: type motionState;
	void AddVelocity(FVector direction, float scale);
	void ResolveMovement(float deltasecounds);
	bool changeMovement(EMotionStates::type newMotionState);
	void MoveHor(float amount);
	//float GetHealth() const;
	void UpdateHealth(float HealthDelta);
	void UpdateScore(float ScoreDelta);



	//wall Movement
	float wallFallSpeed;
	FVector wallnormal;
	bool wallLanded;
	bool touchingWall;
	bool wallJumpInitial;
	FVector wallDirection;
	void OnWall(FVector hitNormal);
	void CheckWallCollision();
	UFUNCTION()
		void CompleteLanding();

	//Gravity
	float GravityAcc;
	float risingGravAcc;
	float maxFallSpeed;
	void AddGravity();

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
private:

		 
		
		IInteractionInterface* Interface = nullptr;
		void OnInteract();

		

		



	
		
};
