// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/GameStates/WBGameStateBase.h"

#include "Wildbound/Wildbound.h"
#include "Engine/World.h"
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
