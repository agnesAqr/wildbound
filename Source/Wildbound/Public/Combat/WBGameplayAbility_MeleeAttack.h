// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Combat/WBGameplayAbility.h"
#include "WBGameplayAbility_MeleeAttack.generated.h"

class UAnimMontage;

UCLASS()
class WILDBOUND_API UWBGameplayAbility_MeleeAttack : public UWBGameplayAbility
{
	GENERATED_BODY()

public:
	UWBGameplayAbility_MeleeAttack();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TObjectPtr<UAnimMontage> AttackMontage;

private:
	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnMontageAborted();
};
