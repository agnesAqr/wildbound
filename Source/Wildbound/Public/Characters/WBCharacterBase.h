// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "WBCharacterBase.generated.h"

class UAbilitySystemComponent;

/** 근접 트레이스 구간을 정의하는 소켓 쌍. 무기 1자루당 항목 1개 */
USTRUCT(BlueprintType)
struct FWBMeleeSocketPair
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee")
	FName StartSocket = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee")
	FName EndSocket = NAME_None;
};

UCLASS(Abstract)
class WILDBOUND_API AWBCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AWBCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	const TArray<FWBMeleeSocketPair>& GetMeleeSocketPairs() const { return MeleeSocketPairs; }

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee")
	TArray<FWBMeleeSocketPair> MeleeSocketPairs;
};
