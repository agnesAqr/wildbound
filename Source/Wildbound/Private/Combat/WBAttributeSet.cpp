// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBAttributeSet.h"

#include "Core/WBGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Wildbound/Wildbound.h"

UWBAttributeSet::UWBAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitStamina(100.0f);
	InitMaxStamina(100.0f);
}

void UWBAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UWBAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWBAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWBAttributeSet, Stamina, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWBAttributeSet, MaxStamina, COND_OwnerOnly, REPNOTIFY_Always);
}

void UWBAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UWBAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UWBAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
}

void UWBAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute != GetHealthAttribute()) return;

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][Attr][Health] IsServer=%d | Target=%s | Delta=%.1f | HP=%.1f/%.1f | Base=%.1f | Instigator=%s"),
		Data.Target.IsOwnerActorAuthoritative() ? 1 : 0,
		*GetNameSafe(Data.Target.GetAvatarActor()),
		Data.EvaluatedData.Magnitude,
		GetHealth(), GetMaxHealth(),
		Data.Target.GetNumericAttributeBase(GetHealthAttribute()),
		*GetNameSafe(Data.EffectSpec.GetEffectContext().GetOriginalInstigator()));

	if (!Data.Target.IsOwnerActorAuthoritative()) return;
	if (GetHealth() > 0.0f) return;
	if (Data.Target.HasMatchingGameplayTag(WBGameplayTags::State_Dead)) return;

	FGameplayEventData Payload;
	Payload.EventTag = WBGameplayTags::Event_State_Down;
	Payload.Instigator = Data.EffectSpec.GetEffectContext().GetOriginalInstigator();
	Payload.Target = Data.Target.GetAvatarActor();

	Data.Target.HandleGameplayEvent(Payload.EventTag, &Payload);
}

void UWBAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWBAttributeSet, Health, OldValue);

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][Attr][OnRep_Health] Owner=%s | %.1f -> %.1f"),
		*GetNameSafe(GetOwningActor()), OldValue.GetCurrentValue(), GetHealth());
}

void UWBAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWBAttributeSet, MaxHealth, OldValue);
}

void UWBAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWBAttributeSet, Stamina, OldValue);
}

void UWBAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWBAttributeSet, MaxStamina, OldValue);
}
