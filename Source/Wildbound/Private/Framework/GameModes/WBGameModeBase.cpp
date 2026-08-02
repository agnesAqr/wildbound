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

/**
 * 서버가 접속 종료를 판정한 사건만 기록한다. 인원수는 GameState가 보고한다.
 * Seamless Travel에서도 PlayerState는 재생성되므로 GameState의 Roster 로그만으로는
 * 실제 이탈과 트래블을 구분할 수 없다 — 이 로그가 그 구분점이다.
 */
void AWBGameModeBase::Logout(AController* Exiting)
{
	const APlayerState* ExitingPS = Exiting ? Exiting->PlayerState : nullptr;
	const FString ExitingName = ExitingPS ? ExitingPS->GetPlayerName() : TEXT("Unknown");

	Super::Logout(Exiting);

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][GameMode][Logout] Player=%s"), *ExitingName);
}
