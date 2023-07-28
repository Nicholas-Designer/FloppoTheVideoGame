// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType, Category = "MotionState Enum")
namespace EMotionStates
{

	enum type
	{
		GROUNDED = 0 UMETA(DisplayName = "Ground"),
		FALLING = 1 UMETA(DisplayName = "Fall"),
		JUMPING = 2 UMETA(DisplayName = "Jump"),
		WALLSIDING = 3 UMETA(DisplayName = "WallSliding"),
		RISING = 4 UMETA(DisplayName = "Rising")
	};


}

class MotionStates
{
public:
	MotionStates();
	~MotionStates();
};