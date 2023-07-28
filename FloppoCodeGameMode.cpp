// Copyright Epic Games, Inc. All Rights Reserved.

#include "FloppoCodeGameMode.h"
#include "FloppoCodeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFloppoCodeGameMode::AFloppoCodeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/SideScrollerCPP/Blueprints/SideScrollerCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
