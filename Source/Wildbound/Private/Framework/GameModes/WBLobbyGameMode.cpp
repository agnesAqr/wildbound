// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/GameModes/WBLobbyGameMode.h"

#include "Framework/GameStates/WBLobbyGameState.h"
#include "Framework/Player/WBPlayerController.h"
#include "Framework/Player/WBPlayerState.h"

AWBLobbyGameMode::AWBLobbyGameMode()
{
	GameStateClass = AWBLobbyGameState::StaticClass();
	PlayerStateClass = AWBPlayerState::StaticClass();
	PlayerControllerClass = AWBPlayerController::StaticClass();
	bUseSeamlessTravel = true;
}
