// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/WBGameplayTags.h"

namespace WBGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Primary, "Ability.Attack.Primary");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Charge, "Ability.Attack.Charge");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dodge, "Ability.Dodge");

	UE_DEFINE_GAMEPLAY_TAG(State_Combat_Attacking, "State.Combat.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(State_Combat_Invulnerable, "State.Combat.Invulnerable");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");

	UE_DEFINE_GAMEPLAY_TAG(Event_Hit_Melee, "Event.Hit.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Event_Input_AttackPrimary, "Event.Input.AttackPrimary");
	UE_DEFINE_GAMEPLAY_TAG(Event_Combo_WindowBegin, "Event.Combo.WindowBegin");
	UE_DEFINE_GAMEPLAY_TAG(Event_Combo_WindowEnd, "Event.Combo.WindowEnd");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Hit_Flesh, "GameplayCue.Hit.Flesh");

	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Attack_Primary, "Cooldown.Attack.Primary");
	UE_DEFINE_GAMEPLAY_TAG(Data_Damage, "Data.Damage");
}
