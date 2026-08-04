// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBGameplayAbility_MeleeAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Core/WBGameplayTags.h"
#include "Wildbound/Wildbound.h"

UWBGameplayAbility_MeleeAttack::UWBGameplayAbility_MeleeAttack()
{
	FGameplayTagContainer AssetTags;
	AssetTags.AddTag(WBGameplayTags::Ability_Attack_Primary);
	SetAssetTags(AssetTags);

	ActivationOwnedTags.AddTag(WBGameplayTags::State_Combat_Attacking);
	ActivationBlockedTags.AddTag(WBGameplayTags::State_Combat_Attacking);
}

void UWBGameplayAbility_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// Super 호출x — 네이티브 경로에선 아무 일도 하지 않고, BP 로직이 추가되면 이중 실행된다.
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!AttackMontage)
	{
		UE_LOG(LogWildbound, Error,
			TEXT("[WB][GA_Melee] AttackMontage 미지정 — GA 블루프린트 확인 필요"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);

	MontageTask->OnCompleted.AddDynamic(this, &UWBGameplayAbility_MeleeAttack::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &UWBGameplayAbility_MeleeAttack::OnMontageAborted);
	MontageTask->OnCancelled.AddDynamic(this, &UWBGameplayAbility_MeleeAttack::OnMontageAborted);
	MontageTask->ReadyForActivation();

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][GA_Melee][Activate] IsServer=%d | IsLocallyControlled=%d | Avatar=%s"),
		ActorInfo->IsNetAuthority() ? 1 : 0,
		ActorInfo->IsLocallyControlled() ? 1 : 0,
		*GetNameSafe(ActorInfo->AvatarActor.Get()));
}

void UWBGameplayAbility_MeleeAttack::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UWBGameplayAbility_MeleeAttack::OnMontageAborted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
