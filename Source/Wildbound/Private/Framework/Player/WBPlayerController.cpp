// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/Player/WBPlayerController.h"

#include "Combat/WBAttributeSet.h"
#include "Engine/World.h"
#include "Framework/Player/WBPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Wildbound/Wildbound.h"

namespace WBMapPath
{
	static const TCHAR* Lobby = TEXT("/Game/Wildbound/Maps/L_Lobby");
	static const TCHAR* Raid  = TEXT("/Game/Wildbound/Maps/L_Raid");
}

void AWBPlayerController::WBTravelRaid()
{
	ServerTravelTo(WBMapPath::Raid);
}

void AWBPlayerController::WBTravelLobby()
{
	ServerTravelTo(WBMapPath::Lobby);
}

void AWBPlayerController::ServerTravelTo(const FString& MapPath)
{
	if (!HasAuthority())
	{
		UE_LOG(LogWildbound, Display,
			TEXT("[WB][PC][Travel] 클라이언트에서 호출 → 서버로 전달: %s"), *MapPath);
		Server_TravelTo(MapPath);
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogWildbound, Error, TEXT("[WB][PC][Travel] World가 없습니다."));
		return;
	}

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][PC][Travel] ServerTravel 시작 → %s"), *MapPath);

	World->ServerTravel(MapPath, /*bAbsolute=*/false);
}

void AWBPlayerController::Server_TravelTo_Implementation(const FString& MapPath)
{
	ServerTravelTo(MapPath);
}

void AWBPlayerController::WBSetProbe(int32 NewValue)
{
	if (!HasAuthority())
	{
		Server_SetProbe(NewValue);
		return;
	}

	Server_SetProbe_Implementation(NewValue);
}

void AWBPlayerController::Server_SetProbe_Implementation(int32 NewValue)
{
	AWBPlayerState* WBPlayerState = GetPlayerState<AWBPlayerState>();
	if (!WBPlayerState)
	{
		UE_LOG(LogWildbound, Error, TEXT("[WB][PC][Probe] PlayerState가 없습니다."));
		return;
	}

	WBPlayerState->SetTravelProbe(NewValue);

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][PC][Probe] 서버가 TravelProbe=%d 로 설정 (Player=%s)"),
		NewValue, *WBPlayerState->GetPlayerName());
}

void AWBPlayerController::WBShowProbe() const
{
	const AWBPlayerState* WBPlayerState = GetPlayerState<AWBPlayerState>();
	if (!WBPlayerState)
	{
		UE_LOG(LogWildbound, Error, TEXT("[WB][PC][Probe] PlayerState가 없습니다."));
		return;
	}

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][PC][Probe] 현재 TravelProbe=%d | Authority=%d | Player=%s"),
		WBPlayerState->GetTravelProbe(),
		HasAuthority() ? 1 : 0,
		*WBPlayerState->GetPlayerName());
}

void AWBPlayerController::WBShowAttributes() const
{
	const UWorld* World = GetWorld();
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	if (!GameState)
	{
		UE_LOG(LogWildbound, Error, TEXT("[WB][PC][Attr] GameState가 없습니다."));
		return;
	}

	const APlayerState* SelfPlayerState = GetPlayerState<APlayerState>();

	for (const APlayerState* EachPlayerState : GameState->PlayerArray)
	{
		const AWBPlayerState* WBPlayerState = Cast<AWBPlayerState>(EachPlayerState);
		if (!WBPlayerState) continue;

		const UWBAttributeSet* Attributes = WBPlayerState->GetAttributeSet();
		if (!Attributes) continue;

		UE_LOG(LogWildbound, Warning,
			TEXT("[WB][PC][Attr] Authority=%d | Target=%s | bSelf=%d | HP=%.0f/%.0f | SP=%.0f/%.0f"),
			HasAuthority() ? 1 : 0,
			*WBPlayerState->GetPlayerName(),
			EachPlayerState == SelfPlayerState ? 1 : 0,
			Attributes->GetHealth(), Attributes->GetMaxHealth(),
			Attributes->GetStamina(), Attributes->GetMaxStamina());
	}
}
