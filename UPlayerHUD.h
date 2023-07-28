// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class FLOPPOCODE_API UUPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public: 
	//Update Hud with current health
	void SetHealth(float CurrentHealth, float MaxHealth);


	//Update Hud with score
	void SetScore(float CurrentScore); 

	//Widget to use to display current Health
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* ScoreText; 

	
};
