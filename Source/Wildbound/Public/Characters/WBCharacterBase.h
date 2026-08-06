// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "WBCharacterBase.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class USkeletalMeshComponent;
class UWBGameplayAbility;

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

	void BeginMeleeTrace(USkeletalMeshComponent* MeshComp, float InTraceRadius, int32 InSampleCount);
	void TickMeleeTrace(USkeletalMeshComponent* MeshComp);
	void EndMeleeTrace();

protected:
	void InitializeDefaultAttributes();
	void GiveDefaultAbilities();

	virtual void HandleMeleeHit(AActor* HitActor, const FHitResult& Hit);

	void GatherTraceSamples(USkeletalMeshComponent* MeshComp, TArray<FVector>& OutSamples) const;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayEffect> DefaultAttributesEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UWBGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee")
	TArray<FWBMeleeSocketPair> MeleeSocketPairs;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> MeleeHitActors;

	TArray<FVector> PreviousTraceSamples;
	float ActiveTraceRadius = 0.0f;
	int32 ActiveSampleCount = 0;
};
