// Fill out your copyright notice in the Description page of Project Settings.


#include "UPlayerHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UUPlayerHUD::SetHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth); 
	}
}



void UUPlayerHUD::SetScore(float CurrentScore)
{
	
	ScoreText->SetText(FText::Format(NSLOCTEXT("PlayerHUDPanel", "ScoreText", "Score: {0}"),CurrentScore));
	
}
