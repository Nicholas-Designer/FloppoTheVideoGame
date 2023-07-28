// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
UENUM(BlueprintType, Category = "AIState Enum")
namespace EAIMovement
{
	enum Type
	{
		PATROL = 0 UMETA(DisplayName = "Patrol"),
		ATTACKING = 1 UMETA(DisplayName = "Attack"),
		DEAD = 2 UMETA(DisplayName = "Dead")
	};
}


class FLOPPOCODE_API AIStates
{
public:
	AIStates();
	~AIStates();
};
