// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/GameStates/WBGameStateBase.h"

#include "Wildbound/Wildbound.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "UObject/Package.h"

void AWBGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (!World) return;

	FString MapName = World->GetMapName();
	MapName.RemoveFromStart(World->StreamingLevelsPrefix);

	const int32 PIEInstanceId = World->GetPackage()->GetPIEInstanceID();
	const FString InstanceName = PIEInstanceId == INDEX_NONE
		? TEXT("Runtime")
		: FString::Printf(TEXT("PIE-%d"), PIEInstanceId);
	const FString ContextName = ToString(GetNetMode()).Replace(TEXT(" "), TEXT(""));

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][GameState][BeginPlay] Context=%s | Instance=%s | Map=%s | Class=%s"),
		*ContextName,
		*InstanceName,
		*MapName,
		*GetClass()->GetName());
}

void AWBGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	LogPlayerRoster(TEXT("Add"), PlayerState);
}

void AWBGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	LogPlayerRoster(TEXT("Remove"), PlayerState);
}

void AWBGameStateBase::LogPlayerRoster(const TCHAR* Event, const APlayerState* PlayerState) const
{
	// 접속 직후에는 아직 이름이 복제되지 않아 비어 있을 수 있다.
	const FString PlayerName = PlayerState ? PlayerState->GetPlayerName() : TEXT("Unknown");
	const FString ContextName = ToString(GetNetMode()).Replace(TEXT(" "), TEXT(""));

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][GameState][Roster][%s] Context=%s | Num=%d | Player=%s"),
		Event, *ContextName, PlayerArray.Num(), *PlayerName);
}
