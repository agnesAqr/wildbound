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

	/** 콤보 순서대로의 몽타주 섹션 이름. 마지막 항목이 콤보 상한이다. */
	UPROPERTY(EditDefaultsOnly, Category = "Melee|Combo")
	TArray<FName> ComboSectionNames;

	/** ComboSectionNames와 같은 순서의 타수별 대미지. */
	UPROPERTY(EditDefaultsOnly, Category = "Melee|Combo", meta = (ClampMin = "0.0"))
	TArray<float> ComboDamageAmounts;

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
