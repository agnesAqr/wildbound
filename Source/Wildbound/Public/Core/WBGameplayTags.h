// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace WBGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Primary);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Charge);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Attacking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Invulnerable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Hit_Melee);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Hit_Flesh);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Attack_Primary);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
}
