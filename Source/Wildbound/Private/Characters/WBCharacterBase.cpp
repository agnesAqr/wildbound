// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/WBCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "Combat/WBGameplayAbility.h"
#include "GameplayEffect.h"

AWBCharacterBase::AWBCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

UAbilitySystemComponent* AWBCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AWBCharacterBase::InitializeDefaultAttributes()
{
	if (!HasAuthority() || !AbilitySystemComponent || !DefaultAttributesEffect) return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle =
		AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributesEffect, 1.0f, EffectContext);
	if (!SpecHandle.IsValid()) return;

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AWBCharacterBase::GiveDefaultAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	for (const TSubclassOf<UWBGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if (!AbilityClass || AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass)) continue;

		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
	}
}
