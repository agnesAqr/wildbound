// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/GameModes/WBRaidGameMode.h"

#include "Framework/GameStates/WBRaidGameState.h"
#include "Framework/Player/WBPlayerController.h"
#include "Framework/Player/WBPlayerState.h"

AWBRaidGameMode::AWBRaidGameMode()
{
	GameStateClass = AWBRaidGameState::StaticClass();
	PlayerStateClass = AWBPlayerState::StaticClass();
	PlayerControllerClass = AWBPlayerController::StaticClass();
}
