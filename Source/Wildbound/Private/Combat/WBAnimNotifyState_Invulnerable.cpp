// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBAnimNotifyState_Invulnerable.h"

#include "AbilitySystemComponent.h"
#include "Characters/WBCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/WBGameplayTags.h"
#include "Wildbound/Wildbound.h"

void UWBAnimNotifyState_Invulnerable::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	SetInvulnerable(MeshComp, true);
}

void UWBAnimNotifyState_Invulnerable::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	SetInvulnerable(MeshComp, false);
}

void UWBAnimNotifyState_Invulnerable::SetInvulnerable(USkeletalMeshComponent* MeshComp, bool bEnable) const
{
	AWBCharacterBase* Character = MeshComp ? Cast<AWBCharacterBase>(MeshComp->GetOwner()) : nullptr;
	UAbilitySystemComponent* AbilitySystemComponent =
		Character ? Character->GetAbilitySystemComponent() : nullptr;
	if (!AbilitySystemComponent) return;

	if (bEnable)
	{
		AbilitySystemComponent->AddLooseGameplayTag(WBGameplayTags::State_Combat_Invulnerable);
	}
	else
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(WBGameplayTags::State_Combat_Invulnerable);
	}

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][Dodge][Invuln] %s | IsServer=%d | Count=%d"),
		bEnable ? TEXT("부여") : TEXT("해제"),
		Character->HasAuthority() ? 1 : 0,
		AbilitySystemComponent->GetGameplayTagCount(WBGameplayTags::State_Combat_Invulnerable));
}
