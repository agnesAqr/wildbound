// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WBGameplayAbility.generated.h"

/**
 * Shared base for every ability in the project. Pins the instancing and network execution policy defaults in one place.
 */
UCLASS(Abstract)
class WILDBOUND_API UWBGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UWBGameplayAbility();
};
