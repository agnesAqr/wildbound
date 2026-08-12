// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbilityTypes.h"
#include "CoreMinimal.h"
#include "Combat/WBGameplayAbility.h"
#include "WBGameplayAbility_MeleeAttack.generated.h"

class UAnimMontage;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FWBMeleeComboStep
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	FName SectionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo", meta = (ClampMin = "0.0"))
	float DamageAmount = 25.0f;
};

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

	/** Combo step data in hit order. */
	UPROPERTY(EditDefaultsOnly, Category = "Melee|Combo")
	TArray<FWBMeleeComboStep> ComboSteps;

private:
	UFUNCTION()
	void OnMeleeHitEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowBegin(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowEnd(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboInputEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnMontageAborted();

	int32 GetCurrentComboIndex() const;
	float GetCurrentComboDamage() const;

	bool bComboWindowOpen = false;
	bool bComboQueued = false;
};
