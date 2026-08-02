// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WBGameModeBase.generated.h"

// Lobby·Raid GameMode의 공통 부모. 판정·규칙은 파생 클래스에, 복제 상태는 GameState에 둔다.
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
