// Copyright Epic Games, Inc. All Rights Reserved.

#include "FloppoCodeCharacter.h"
#include "UPlayerHUD.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"



AFloppoCodeCharacter::AFloppoCodeCharacter()
{
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 100.f;
	CameraBoom->SocketOffset = FVector(0.f,-1000.f,0.f);
	CameraBoom->SetRelativeRotation(FRotator(0.f,180.f,0.f));

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	//Interaction Box
	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Box"));
	InteractionBox->SetupAttachment(RootComponent);


	//HUD
	PlayerHUDClass = nullptr;
	PlayerHUD = nullptr;

	//Health
	MaxHealth = 100.f;

	Health = MaxHealth;

	HasCollected = false;
	//Score
	Score = 0.f;
}

void AFloppoCodeCharacter::BeginPlay()
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
}

void AFloppoCodeCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromParent();
		PlayerHUD = nullptr;
	}
}
void AFloppoCodeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AFloppoCodeCharacter, Health, COND_OwnerOnly);
}

void AFloppoCodeCharacter::Tick(float DeltaTime)
{
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


//////////////////////////////////////////////////////////////////////////
// Input

void AFloppoCodeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis(FName("MoveHorizontal"), this, &AFloppoCodeCharacter::MoveHorz);


	//interact with object
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFloppoCodeCharacter::OnInteract);


	PlayerInputComponent->BindTouch(IE_Pressed, this, &AFloppoCodeCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AFloppoCodeCharacter::TouchStopped);
}

void AFloppoCodeCharacter::MoveHorz(float Value)
{
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f); 

	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value); 

}




void AFloppoCodeCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// jump on any touch
	Jump();
}

void AFloppoCodeCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	StopJumping();
}


void AFloppoCodeCharacter::OnRepHealth()
{
	if (PlayerHUD)
	{
		PlayerHUD->SetHealth(Health, MaxHealth);
	}
}


void AFloppoCodeCharacter::UpdateHealth(float HealthDelta)
{
	Health = FMath::Clamp(Health + HealthDelta, 0.f, MaxHealth);

	if (Health == 0.f)
	{
		// game Over
	}
}

void AFloppoCodeCharacter::UpdateScore(float ScoreDelta)
{

}


void AFloppoCodeCharacter::OnInteract()
{
	if (Interface)
	{
		HasCollected = true;
		Interface->InteractWithMe();

	}
}
