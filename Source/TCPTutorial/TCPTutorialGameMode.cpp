// Copyright Epic Games, Inc. All Rights Reserved.

#include "TCPTutorialGameMode.h"
#include "TCPTutorialCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATCPTutorialGameMode::ATCPTutorialGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
