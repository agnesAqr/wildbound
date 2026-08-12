// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbilityTypes.h"
#include "CoreMinimal.h"
#include "Combat/WBGameplayAbility.h"
#include "WBGameplayAbility_Down.generated.h"

class UAnimMontage;

UCLASS()
class WILDBOUND_API UWBGameplayAbility_Down : public UWBGameplayAbility
{
	GENERATED_BODY()

public:
	UWBGameplayAbility_Down();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Down")
	TObjectPtr<UAnimMontage> DownMontage;
};
