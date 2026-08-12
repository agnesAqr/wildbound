// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBGameplayAbility.h"

#include "Core/WBGameplayTags.h"

UWBGameplayAbility::UWBGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	ActivationBlockedTags.AddTag(WBGameplayTags::State_Dead);
}
