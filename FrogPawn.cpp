// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogPawn.h"
#include "Components/SphereComponent.h"
#include "GameFrameWork/FloatingPawnMovement.h"
#include "UPlayerHUD.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"

// Sets default values
AFrogPawn::AFrogPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; 
	capsule = CreateDefaultSubobject<USphereComponent>(TEXT("Frog_SpeherComponent"));
	pawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Frog_PawnMovemnt"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Frog_CameraComponent"));
	
	Died = false;
	//Interaction Box
	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Box"));
	InteractionBox->SetupAttachment(RootComponent);
	
	//HUD
	PlayerHUDClass = nullptr;
	PlayerHUD = nullptr;
	//Health
	MaxHealth = 100.f; 

	Health = MaxHealth;


	//Score
	Score = 0.f;

}

// Called when the game starts or when spawned
void AFrogPawn::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled() && PlayerHUDClass)
	{
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		check(controller);
		PlayerHUD = CreateWidget<UUPlayerHUD>(controller, PlayerHUDClass); 
		check(PlayerHUD);
		PlayerHUD->AddToPlayerScreen();
		PlayerHUD->SetHealth(Health, MaxHealth);
		PlayerHUD->SetScore(Score);
	}

	


	/*controller->Possess(this);
	controller->SetViewTarget(this);*/


	camera->SetRelativeLocation(FVector(-500, 0, 0));


	//Deafult Values
	
	walkSpeed = 100.f;
	maxFallSpeed = -1000.f;
	wallFallSpeed = -300.f;



	jumpAcc = 100.f;//100
	jumpMax = .233333f; // .233333
	jumpSpeed = 100.f; //100 
	hasJumped = false;


	GravityAcc = 3000.f; //3000
	risingGravAcc = 2100.f;//2100


	wallJumpInitial = false;

	capsule->SetSphereRadius(50.f);
	capsule->SetHiddenInGame(false);
	capsule->SetSimulatePhysics(true);
	capsule->SetEnableGravity(false);
	capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	capsule->OnComponentHit.AddDynamic(this, &AFrogPawn::OnHit);

	//apply damage to player
	AActor* MyOwner = GetOwner();
	
	
	MyOwner->OnTakeAnyDamage.AddDynamic(this, &AFrogPawn::HandleTakenAnyDamage);

	

	UE_LOG(LogTemp, Warning, TEXT("Health is: %s"), *FString::SanitizeFloat(Health));
	

	
}

void AFrogPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromParent();
		PlayerHUD = nullptr; 
	}
}

void AFrogPawn::HandleTakenAnyDamage(AActor * DamagedActor, float damage, const UDamageType * DamageType, AController * InstagatedBy, AActor * DamageCauser)
{
	if (damage <= 0)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));
	Health = FMath::Clamp(Health - damage, 0.0f, MaxHealth);
	
}

// Called every frame
void AFrogPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddGravity();
	Jumping(DeltaTime);
	CheckWallCollision();

	ResolveMovement(DeltaTime);

	TArray<AActor*>CollectableActors;
	InteractionBox->GetOverlappingActors(CollectableActors);

	if (CollectableActors.Num() == 0)
	{
		if (Interface)
		{
			Interface->HideInteractionWidget();
			Interface = nullptr;
		}
		return;
	}


	AActor* ClosestActor = CollectableActors[0];
	for (auto CurrentActor : CollectableActors)
	{
		if (GetDistanceTo(CurrentActor) < GetDistanceTo(ClosestActor))
		{
			ClosestActor = CurrentActor; 
		}
	}

	if (Interface)
	{
		Interface->HideInteractionWidget();
	}

	Interface = Cast<IInteractionInterface>(ClosestActor); 

	if (Interface)
	{
		Interface->ShowInteractionWidget();
	}


}

bool AFrogPawn::changeMovement(EMotionStates::type newMotionState)
{
	EMotionStates::type oldMotionState = motionState;

	switch (newMotionState)	
	{
	case EMotionStates::GROUNDED:
		motionState = newMotionState;
		break;

	case EMotionStates::JUMPING:
		motionState = newMotionState;
		break;
	case EMotionStates::RISING:
		motionState = newMotionState;
		break;

	case EMotionStates::FALLING:
		if (oldMotionState != EMotionStates::JUMPING)
		{
			motionState = newMotionState;
		}
		break;


	case EMotionStates::WALLSIDING:
		if (oldMotionState == EMotionStates::FALLING)
		{
			motionState = newMotionState;
		}
		break;
	}

	if (oldMotionState != newMotionState)
	{
		if (oldMotionState == EMotionStates::WALLSIDING)
		{
			this->wallLanded = false;
		}
		if (newMotionState == EMotionStates::WALLSIDING)
		{
			this->wallLanded = true;
			//UKismetSystemLibrary::K2_SetTimer(this, TEXT("CompleteWallLanding"), .1f, false);
		}
	}

	return oldMotionState != newMotionState; 
}

// Called to bind functionality to input
void AFrogPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	PlayerInputComponent->BindAxis(FName("MoveHorizontal"), this, &AFrogPawn::MoveHor);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &AFrogPawn::StartJump);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Released, this, &AFrogPawn::InterruptJump);


	//interact with object
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFrogPawn::OnInteract);
	
}

void AFrogPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AFrogPawn, Health, COND_OwnerOnly); 
	 

}

void AFrogPawn::AddVelocity(FVector direction, float scale)
{
	this->AddVol += direction * scale; 
}


void AFrogPawn::ResolveMovement(float deltaTime)
{

	if (motionState == EMotionStates::JUMPING || motionState == EMotionStates::RISING) 
	{
		if (this->Vol.Z <= 0.01f)
		{
			changeMovement(EMotionStates::FALLING);
		}
	}

	
	FVector initialLocation = this->GetActorLocation();

	FVector totalVelocity = (FMath::Sign(this->AddVol.Y) == FMath::Sign(this->Vol.Y)) ? this->AddVol + this->Vol : this->AddVol;
	totalVelocity = FMath::Sign(this->Vol.Y) == 0 ? this->AddVol + this->Vol : totalVelocity;

	//Wall clamping/ sliding
	float VerticalClamp = (motionState == EMotionStates::GROUNDED || motionState == EMotionStates::FALLING) ? this->maxFallSpeed : 0.f; 
	VerticalClamp = (motionState == EMotionStates::WALLSIDING) ? this->wallFallSpeed : VerticalClamp;
	VerticalClamp = (motionState == EMotionStates::WALLSIDING && this->wallLanded) ? 0 : VerticalClamp;
	
	

	FVector newLocation = deltaTime * totalVelocity + initialLocation;

	FHitResult hitResult;
	SetActorLocation(newLocation, true, &hitResult, ETeleportType::None);
	while (hitResult.bBlockingHit) {
		FVector displacement = hitResult.TraceEnd - hitResult.TraceStart;
		FVector displacementAdjustment = displacement - FVector::DotProduct(displacement, hitResult.ImpactNormal) * hitResult.ImpactNormal + hitResult.ImpactNormal * 0.0125;
		FVector displacedActor = this->GetActorLocation() + displacementAdjustment;
		displacedActor = FVector(0.f, displacedActor.Y, displacedActor.Z);
		SetActorLocation(displacedActor, true, &hitResult, ETeleportType::None);
	}

	this->Vol = (this->GetActorLocation() - initialLocation) / deltaTime;
	this->AddVol = FVector::ZeroVector;


}

void AFrogPawn::AddGravity()
{
	float addedGravity = GravityAcc;
	switch (motionState)
	{
	case EMotionStates::GROUNDED:
			break;

	case EMotionStates::JUMPING:
		addedGravity = 0.f;
		break;

	case EMotionStates::RISING:
		addedGravity = risingGravAcc;
		break;

	case EMotionStates::WALLSIDING:
		AddVelocity(FVector::RightVector, FMath::Sign(this->wallnormal.Y) * -1.f);
		break;
	}

	AddVelocity(FVector::DownVector, addedGravity); 
}

void AFrogPawn::MoveHor(float amount)
{

	if (!this->wallJumpInitial)
	{
		if (FMath::Abs(amount) != 0)
		{
			this->lookDirection = amount > 0 ? FVector::RightVector : FVector::LeftVector;
		}

		AddVelocity(amount * FVector::RightVector, walkSpeed);
	}

	

}

void AFrogPawn::OnRepHealth()
{
	if (PlayerHUD)
	{
		PlayerHUD->SetHealth(Health, MaxHealth);
	}
}

void AFrogPawn::UpdateHealth(float healthdelta)
{
	Health = FMath::Clamp(Health + healthdelta, 0.f, MaxHealth);

	if (Health == 0.f)
	{
		// game Over
	}
}

void AFrogPawn::UpdateScore(float Scoredelta)
{
	
}





void AFrogPawn::CheckWallCollision()
{
	FVector rightSphereCenter = this->GetActorLocation() + 50 * FVector::RightVector;
	FVector leftSphereCenter = this->GetActorLocation() + 50 * FVector::LeftVector;
	FCollisionShape collisionSphere = FCollisionShape::MakeSphere(5.f);


	TArray<TEnumAsByte<EObjectTypeQuery>> collisionParams;
	collisionParams.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	collisionParams.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

	TArray<AActor*> ignoredActors;
	FHitResult RightOutHit;
	FHitResult LeftOutHit;
	bool rightHit = UKismetSystemLibrary::SphereTraceSingleForObjects(this, rightSphereCenter, rightSphereCenter, collisionSphere.GetSphereRadius(),
		collisionParams,false, ignoredActors, EDrawDebugTrace::ForDuration, RightOutHit, true);
	bool leftHit = UKismetSystemLibrary::SphereTraceSingleForObjects(this, leftSphereCenter, leftSphereCenter, collisionSphere.GetSphereRadius(),
		collisionParams, false, ignoredActors, EDrawDebugTrace::ForDuration, LeftOutHit, true); 


	if (RightOutHit.bBlockingHit)
	{
		if (FMath::Abs(FVector::DotProduct(RightOutHit.ImpactNormal, FVector::RightVector)) >= 0.71f)
		{
			this->wallDirection = FVector::RightVector * FMath::Sign(RightOutHit.ImpactNormal.Y);
		}
	}
	if (LeftOutHit.bBlockingHit)
	{
		if (FMath::Abs(FVector::DotProduct(LeftOutHit.ImpactNormal, FVector::LeftVector)) >= 0.71f)
		{
			this->wallDirection = FVector::RightVector * FMath::Sign(LeftOutHit.ImpactNormal.Y);
		}
	}


	this->touchingWall == rightHit || leftHit; 

}

void AFrogPawn::CompleteLanding()
{
	this->wallLanded = false;
}


void AFrogPawn::StartJump()
{
	this->jumpTimer = 0;
	if (motionState == EMotionStates::GROUNDED)
	{
		changeMovement(EMotionStates::JUMPING);
	}	
	else
	{
		if (this->touchingWall)
		{
			changeMovement(EMotionStates::JUMPING);
			this->wallJumpInitial = true;
			this->lookDirection = -1 * this->wallDirection;
			this->Vol = FVector::ZeroVector; 
		}

		else
		{
			if (IsInAir())
			{
				changeMovement(EMotionStates::JUMPING);
			}
		}
	}
}


void AFrogPawn::Jumping(float deltasecounds)
{
	if (motionState == EMotionStates::JUMPING)
	{
		if (this->wallJumpInitial)
		{
			FVector wallJumpDirection = FVector::UpVector + this->wallDirection;
			AddVelocity(wallDirection, this->jumpAcc);
		}
		else
		{
			AddVelocity(FVector::UpVector, this->jumpAcc);

		}
		this->jumpTimer += deltasecounds;
		if (this->jumpTimer >= this->jumpMax)
		{
			this->jumpTimer = 0;
			StopJump(false);
		}

		if (this->jumpTimer >= this->jumpMax / 2)
		{
			this->wallJumpInitial = false;
		}

	}
}

void AFrogPawn::StopJump(bool sharp)
{
	if (motionState == EMotionStates::JUMPING)
	{
		this->wallJumpInitial = false;
		changeMovement(EMotionStates::RISING);
		if (sharp)
		{
			AddVelocity(FVector::DownVector, FMath::Abs(this->Vol.Z / (GetWorld()->GetDeltaSeconds() * 1.5)));
		}
	}
}

void AFrogPawn::Jump()
{
	StartJump();
}

void AFrogPawn::InterruptJump()
{
	StopJump(true);
}


void AFrogPawn::Ungrounded()
{
	if (!IsInAir())
	{
		changeMovement(EMotionStates::FALLING); 
	}
}

bool AFrogPawn::IsInAir()
{


	return (motionState == EMotionStates::JUMPING || motionState == EMotionStates::FALLING || motionState == EMotionStates::RISING);
}

void AFrogPawn::Grounded()
{
	changeMovement(EMotionStates::GROUNDED);
	UKismetSystemLibrary::K2_SetTimer(this, TEXT("Ungrounded"), .000001f, false); // reset frame until off the ground
}


void AFrogPawn::OnWall(FVector hitNormal)
{
	changeMovement(EMotionStates::WALLSIDING);
	this->wallnormal = hitNormal;
	UKismetSystemLibrary::K2_SetTimer(this, TEXT("Ungrounded"), .000001f, false); // reset frame until off the wall
}

void AFrogPawn::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (FVector::DotProduct(Hit.ImpactNormal, FVector::UpVector) >= 0.71f)
	{
		Grounded(); 
	}
	else if (FMath::Abs(FVector::DotProduct(Hit.ImpactNormal, FVector::RightVector)) >= 0.71f)
	{
		
		OnWall(Hit.ImpactNormal); 
	}
}



void AFrogPawn::OnInteract()
{
	if (Interface)
	{
		Interface->InteractWithMe();
		
	}
}



