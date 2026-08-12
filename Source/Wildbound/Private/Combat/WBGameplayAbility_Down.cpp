// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBGameplayAbility_Down.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Core/WBGameplayTags.h"
#include "Wildbound/Wildbound.h"

UWBGameplayAbility_Down::UWBGameplayAbility_Down()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	ActivationOwnedTags.AddTag(WBGameplayTags::State_Dead);

	FAbilityTriggerData Trigger;
	Trigger.TriggerTag = WBGameplayTags::Event_State_Down;
	Trigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(Trigger);
}

void UWBGameplayAbility_Down::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo())
	{
		SourceASC->CancelAllAbilities(this);
	}

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][GA_Down][Activate] IsServer=%d | Avatar=%s"),
		ActorInfo->IsNetAuthority() ? 1 : 0,
		*GetNameSafe(ActorInfo->AvatarActor.Get()));

	if (!DownMontage)
	{
		UE_LOG(LogWildbound, Error,
			TEXT("[WB][GA_Down] DownMontage 미지정 — GA 블루프린트 확인 필요"));
		return;
	}

	// The ability deliberately never ends: DownLoop holds the pose until W5 adds respawn.
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DownMontage);
	MontageTask->ReadyForActivation();
}
