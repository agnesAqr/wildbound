// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBGameplayAbility_Charge.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Core/WBGameplayTags.h"
#include "GameplayEffect.h"
#include "Wildbound/Wildbound.h"

UWBGameplayAbility_Charge::UWBGameplayAbility_Charge()
{
	FGameplayTagContainer AssetTags;
	AssetTags.AddTag(WBGameplayTags::Ability_Attack_Charge);
	SetAssetTags(AssetTags);

	ActivationOwnedTags.AddTag(WBGameplayTags::State_Combat_Charging);
	ActivationBlockedTags.AddTag(WBGameplayTags::State_Combat_Charging);
	ActivationBlockedTags.AddTag(WBGameplayTags::State_Combat_Attacking);
	ActivationBlockedTags.AddTag(WBGameplayTags::State_Combat_Dodging);
}

void UWBGameplayAbility_Charge::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!ChargeMontage)
	{
		UE_LOG(LogWildbound, Error,
			TEXT("[WB][GA_Charge] ChargeMontage 미지정 — GA 블루프린트 확인 필요"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ChargeHeldSeconds = 0.0f;

	UAbilityTask_WaitGameplayEvent* HitEventTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, WBGameplayTags::Event_Hit_Melee);
	HitEventTask->EventReceived.AddDynamic(this, &UWBGameplayAbility_Charge::OnMeleeHitEvent);
	HitEventTask->ReadyForActivation();

	UAbilityTask_WaitInputRelease* ReleaseTask =
		UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	ReleaseTask->OnRelease.AddDynamic(this, &UWBGameplayAbility_Charge::OnInputReleased);
	ReleaseTask->ReadyForActivation();

	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ChargeMontage);

	MontageTask->OnCompleted.AddDynamic(this, &UWBGameplayAbility_Charge::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &UWBGameplayAbility_Charge::OnMontageAborted);
	MontageTask->OnCancelled.AddDynamic(this, &UWBGameplayAbility_Charge::OnMontageAborted);
	MontageTask->ReadyForActivation();

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][GA_Charge][Activate] IsServer=%d | IsLocallyControlled=%d"),
		ActorInfo->IsNetAuthority() ? 1 : 0,
		ActorInfo->IsLocallyControlled() ? 1 : 0);
}

void UWBGameplayAbility_Charge::OnInputReleased(float TimeHeld)
{
	ChargeHeldSeconds = TimeHeld;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	SourceASC->CurrentMontageJumpToSection(ChargeReleaseSection);

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][GA_Charge][Release] IsServer=%d | Held=%.2fs | Damage=%.0f"),
		CurrentActorInfo && CurrentActorInfo->IsNetAuthority() ? 1 : 0,
		ChargeHeldSeconds, GetChargeDamage());
}

void UWBGameplayAbility_Charge::OnMeleeHitEvent(FGameplayEventData Payload)
{
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Payload.Target.Get());

	if (!SourceASC || !TargetASC || !DamageGameplayEffect)
	{
		UE_LOG(LogWildbound, Error,
			TEXT("[WB][GA_Charge][Damage] 적용 불가 | Target=%s | TargetASC=%d | GE=%d"),
			*GetNameSafe(Payload.Target.Get()), TargetASC != nullptr, DamageGameplayEffect != nullptr);
		return;
	}

	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle =
		SourceASC->MakeOutgoingSpec(DamageGameplayEffect, GetAbilityLevel(), EffectContext);
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(WBGameplayTags::Data_Damage, -GetChargeDamage());

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

float UWBGameplayAbility_Charge::GetChargeDamage() const
{
	const float Span = FMath::Max(MaxChargeSeconds - MinChargeSeconds, KINDA_SMALL_NUMBER);
	const float Alpha = FMath::Clamp((ChargeHeldSeconds - MinChargeSeconds) / Span, 0.0f, 1.0f);

	return FMath::Lerp(MinChargeDamage, MaxChargeDamage, Alpha);
}

void UWBGameplayAbility_Charge::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UWBGameplayAbility_Charge::OnMontageAborted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
