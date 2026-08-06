// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbilityTypes.h"
#include "CoreMinimal.h"
#include "Combat/WBGameplayAbility.h"
#include "WBGameplayAbility_MeleeAttack.generated.h"

class UAnimMontage;
class UGameplayEffect;

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

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Melee", meta = (ClampMin = "0.0"))
	float DamageAmount = 25.0f;

private:
	UFUNCTION()
	void OnMeleeHitEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnMontageAborted();
};
