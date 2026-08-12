// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbilityTypes.h"
#include "CoreMinimal.h"
#include "Combat/WBGameplayAbility.h"
#include "WBGameplayAbility_Charge.generated.h"

class UAnimMontage;
class UGameplayEffect;

UCLASS()
class WILDBOUND_API UWBGameplayAbility_Charge : public UWBGameplayAbility
{
	GENERATED_BODY()

public:
	UWBGameplayAbility_Charge();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Charge")
	TObjectPtr<UAnimMontage> ChargeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Charge")
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Charge")
	FName ChargeReleaseSection = TEXT("ChargeRelease");

	/** Hold time that still counts as the weakest charge, in seconds. */
	UPROPERTY(EditDefaultsOnly, Category = "Charge", meta = (ClampMin = "0.0"))
	float MinChargeSeconds = 0.0f;

	/** Hold time at which damage stops growing, in seconds. */
	UPROPERTY(EditDefaultsOnly, Category = "Charge", meta = (ClampMin = "0.1"))
	float MaxChargeSeconds = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Charge", meta = (ClampMin = "0.0"))
	float MinChargeDamage = 40.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Charge", meta = (ClampMin = "0.0"))
	float MaxChargeDamage = 110.0f;

private:
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	UFUNCTION()
	void OnMeleeHitEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnMontageAborted();

	float GetChargeDamage() const;

	float ChargeHeldSeconds = 0.0f;
};
