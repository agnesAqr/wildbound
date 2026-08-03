// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/WBCharacterBase.h"

#include "AbilitySystemComponent.h"

AWBCharacterBase::AWBCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

UAbilitySystemComponent* AWBCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
