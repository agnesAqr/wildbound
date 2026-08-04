// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBGameplayAbility.h"

UWBGameplayAbility::UWBGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}
