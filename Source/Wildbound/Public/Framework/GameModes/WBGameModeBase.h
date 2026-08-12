// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WBGameModeBase.generated.h"

// Shared parent of the Lobby and Raid game modes. Rules live in the derived classes, replicated state in GameState.
UCLASS()
class WILDBOUND_API AWBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWBGameModeBase();

	virtual void PostSeamlessTravel() override;

	virtual void Logout(AController* Exiting) override;

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};
