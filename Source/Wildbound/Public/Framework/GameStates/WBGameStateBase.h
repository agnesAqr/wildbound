// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WBGameStateBase.generated.h"

UCLASS()
class WILDBOUND_API AWBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	/**
	 * The head count is replicated state, so GameState is what reports it.
	 * It is only accurate when counted right after PlayerArray actually changes — at
	 * AGameModeBase::Logout the leaving PlayerState is still in the array and the count reads one too high.
	 * These are called on every client as well as the server, so the log carries its execution context.
	 */
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

protected:
	virtual void BeginPlay() override;

private:
	void LogPlayerRoster(const TCHAR* Event, const APlayerState* PlayerState) const;
};
