// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "JH302GameMode.h"
#include "JH302Character.h"
#include "UObject/ConstructorHelpers.h"

AJH302GameMode::AJH302GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
