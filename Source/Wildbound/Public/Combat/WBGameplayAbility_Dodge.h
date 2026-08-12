// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbilityTypes.h"
#include "CoreMinimal.h"
#include "Combat/WBGameplayAbility.h"
#include "WBGameplayAbility_Dodge.generated.h"

class UAnimMontage;
class UCurveFloat;

UCLASS()
class WILDBOUND_API UWBGameplayAbility_Dodge : public UWBGameplayAbility
{
	GENERATED_BODY()

public:
	UWBGameplayAbility_Dodge();

protected:
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeMontage;

	/** Backward speed in cm/s, held for the whole move duration. */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge", meta = (ClampMin = "0.0"))
	float DodgeSpeed = 400.0f;

	/** Seconds the dodge keeps moving. 0 uses the montage length. */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge", meta = (ClampMin = "0.0"))
	float DodgeMoveDuration = 0.0f;

	/** Speed falloff over the move duration. X is 0~1 normalized time, Y is 0~1 of Dodge Speed. */
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	TObjectPtr<UCurveFloat> DodgeSpeedCurve;

private:
	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnMontageAborted();
};
