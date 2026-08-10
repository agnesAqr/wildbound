// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBGameplayAbility_MeleeAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "Core/WBGameplayTags.h"
#include "GameplayEffect.h"
#include "Wildbound/Wildbound.h"

UWBGameplayAbility_MeleeAttack::UWBGameplayAbility_MeleeAttack()
{
	FGameplayTagContainer AssetTags;
	AssetTags.AddTag(WBGameplayTags::Ability_Attack_Primary);
	SetAssetTags(AssetTags);

	ActivationOwnedTags.AddTag(WBGameplayTags::State_Combat_Attacking);
	ActivationBlockedTags.AddTag(WBGameplayTags::State_Combat_Attacking);

	ComboSectionNames = { TEXT("Combo1"), TEXT("Combo2"), TEXT("Combo3"), TEXT("Combo4") };
	ComboDamageAmounts = { 25.0f, 25.0f, 30.0f, 40.0f };
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

	UAbilityTask_WaitGameplayEvent* HitEventTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, WBGameplayTags::Event_Hit_Melee);

	HitEventTask->EventReceived.AddDynamic(this, &UWBGameplayAbility_MeleeAttack::OnMeleeHitEvent);
	HitEventTask->ReadyForActivation();

	bComboWindowOpen = false;
	bComboQueued = false;

	UAbilityTask_WaitGameplayEvent* WindowBeginTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, WBGameplayTags::Event_Combo_WindowBegin);
	WindowBeginTask->EventReceived.AddDynamic(this, &UWBGameplayAbility_MeleeAttack::OnComboWindowBegin);
	WindowBeginTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WindowEndTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, WBGameplayTags::Event_Combo_WindowEnd);
	WindowEndTask->EventReceived.AddDynamic(this, &UWBGameplayAbility_MeleeAttack::OnComboWindowEnd);
	WindowEndTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* ComboInputTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, WBGameplayTags::Event_Input_AttackPrimary);
	ComboInputTask->EventReceived.AddDynamic(this, &UWBGameplayAbility_MeleeAttack::OnComboInputEvent);
	ComboInputTask->ReadyForActivation();

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

void UWBGameplayAbility_MeleeAttack::OnMeleeHitEvent(FGameplayEventData Payload)
{
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Payload.Target.Get());

	if (!SourceASC || !TargetASC || !DamageGameplayEffect)
	{
		UE_LOG(LogWildbound, Error,
			TEXT("[WB][GA_Melee][Damage] 적용 불가 | Target=%s | TargetASC=%d | GE=%d"),
			*GetNameSafe(Payload.Target.Get()), TargetASC != nullptr, DamageGameplayEffect != nullptr);
		return;
	}

	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle =
		SourceASC->MakeOutgoingSpec(DamageGameplayEffect, GetAbilityLevel(), EffectContext);
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(WBGameplayTags::Data_Damage, -GetCurrentComboDamage());

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

void UWBGameplayAbility_MeleeAttack::OnComboWindowBegin(FGameplayEventData Payload)
{
	bComboWindowOpen = true;
	bComboQueued = false;
}

void UWBGameplayAbility_MeleeAttack::OnComboWindowEnd(FGameplayEventData Payload)
{
	bComboWindowOpen = false;
}

void UWBGameplayAbility_MeleeAttack::OnComboInputEvent(FGameplayEventData Payload)
{
	if (!bComboWindowOpen || bComboQueued) return;

	const int32 ComboIndex = GetCurrentComboIndex();
	if (ComboIndex == INDEX_NONE || !ComboSectionNames.IsValidIndex(ComboIndex + 1)) return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	SourceASC->CurrentMontageSetNextSectionName(
		ComboSectionNames[ComboIndex], ComboSectionNames[ComboIndex + 1]);
	bComboQueued = true;

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][GA_Melee][Combo] 예약 | IsServer=%d | %s -> %s"),
		CurrentActorInfo && CurrentActorInfo->IsNetAuthority() ? 1 : 0,
		*ComboSectionNames[ComboIndex].ToString(),
		*ComboSectionNames[ComboIndex + 1].ToString());
}

int32 UWBGameplayAbility_MeleeAttack::GetCurrentComboIndex() const
{
	UAnimInstance* AnimInstance = CurrentActorInfo ? CurrentActorInfo->GetAnimInstance() : nullptr;
	if (!AnimInstance || !AttackMontage) return INDEX_NONE;

	return ComboSectionNames.IndexOfByKey(AnimInstance->Montage_GetCurrentSection(AttackMontage));
}

float UWBGameplayAbility_MeleeAttack::GetCurrentComboDamage() const
{
	const int32 ComboIndex = GetCurrentComboIndex();
	if (ComboDamageAmounts.IsValidIndex(ComboIndex))
	{
		return ComboDamageAmounts[ComboIndex];
	}

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][GA_Melee][Damage] 타수 판독 실패 | Index=%d | 대미지 배열 길이=%d"),
		ComboIndex, ComboDamageAmounts.Num());

	return 0.0f;
}

void UWBGameplayAbility_MeleeAttack::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UWBGameplayAbility_MeleeAttack::OnMontageAborted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
