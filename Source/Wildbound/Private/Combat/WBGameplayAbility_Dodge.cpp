// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBGameplayAbility_Dodge.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Combat/WBAttributeSet.h"
#include "Core/WBGameplayTags.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/RootMotionSource.h"
#include "Wildbound/Wildbound.h"

static float GetStamina(const FGameplayAbilityActorInfo* ActorInfo)
{
	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;

	return ASC ? ASC->GetNumericAttribute(UWBAttributeSet::GetStaminaAttribute()) : -1.0f;
}

UWBGameplayAbility_Dodge::UWBGameplayAbility_Dodge()
{
	FGameplayTagContainer AssetTags;
	AssetTags.AddTag(WBGameplayTags::Ability_Dodge);
	SetAssetTags(AssetTags);

	ActivationOwnedTags.AddTag(WBGameplayTags::State_Combat_Dodging);
	ActivationBlockedTags.AddTag(WBGameplayTags::State_Combat_Dodging);
	ActivationBlockedTags.AddTag(WBGameplayTags::State_Combat_Attacking);
}

bool UWBGameplayAbility_Dodge::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags)) return true;

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][GA_Dodge][Cost] 거부 | IsServer=%d | SP=%.1f"),
		ActorInfo && ActorInfo->IsNetAuthority() ? 1 : 0, GetStamina(ActorInfo));

	return false;
}

void UWBGameplayAbility_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!DodgeMontage)
	{
		UE_LOG(LogWildbound, Error,
			TEXT("[WB][GA_Dodge] DodgeMontage 미지정 — GA 블루프린트 확인 필요"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DodgeMontage);

	MontageTask->OnCompleted.AddDynamic(this, &UWBGameplayAbility_Dodge::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &UWBGameplayAbility_Dodge::OnMontageAborted);
	MontageTask->OnCancelled.AddDynamic(this, &UWBGameplayAbility_Dodge::OnMontageAborted);
	MontageTask->ReadyForActivation();

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	const float MoveDuration =
		DodgeMoveDuration > 0.0f ? DodgeMoveDuration : DodgeMontage->GetPlayLength();

	if (AvatarActor && DodgeSpeed > 0.0f && MoveDuration > 0.0f)
	{
		UAbilityTask_ApplyRootMotionConstantForce* MoveTask =
			UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
				this, NAME_None, -AvatarActor->GetActorForwardVector(), DodgeSpeed, MoveDuration,
				false, DodgeSpeedCurve,
				ERootMotionFinishVelocityMode::SetVelocity, FVector::ZeroVector, 0.0f, false);

		MoveTask->ReadyForActivation();
	}

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][GA_Dodge][Activate] IsServer=%d | IsLocallyControlled=%d | SP=%.1f"),
		ActorInfo->IsNetAuthority() ? 1 : 0,
		ActorInfo->IsLocallyControlled() ? 1 : 0,
		GetStamina(ActorInfo));
}

void UWBGameplayAbility_Dodge::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UWBGameplayAbility_Dodge::OnMontageAborted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
