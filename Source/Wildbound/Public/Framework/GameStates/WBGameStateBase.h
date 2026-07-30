// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WBGameStateBase.generated.h"

UCLASS()
class WILDBOUND_API AWBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
