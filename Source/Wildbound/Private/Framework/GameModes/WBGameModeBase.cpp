// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/GameModes/WBGameModeBase.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Wildbound/Wildbound.h"

AWBGameModeBase::AWBGameModeBase()
{
	bUseSeamlessTravel = true;
}

void AWBGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][GameMode][InitGame] Class=%s | Map=%s | SeamlessTravel=%d"),
		*GetClass()->GetName(), *MapName, bUseSeamlessTravel ? 1 : 0);
}

void AWBGameModeBase::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][GameMode][PostSeamlessTravel] Class=%s | SEAMLESS OK"),
		*GetClass()->GetName());
}

void AWBGameModeBase::Logout(AController* Exiting)
{
	const APlayerState* ExitingPS = Exiting ? Exiting->PlayerState : nullptr;
	const FString ExitingName = ExitingPS ? ExitingPS->GetPlayerName() : TEXT("Unknown");

	Super::Logout(Exiting);

	const int32 Remaining = GameState ? GameState->PlayerArray.Num() : -1;

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][GameMode][Logout] Player=%s | Remaining=%d"),
		*ExitingName, Remaining);
}
