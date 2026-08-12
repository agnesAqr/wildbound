// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WBTargetDummy.generated.h"

class UAbilitySystemComponent;
class UCapsuleComponent;
class UGameplayEffect;
class UStaticMeshComponent;
class UWBAttributeSet;

UCLASS()
class WILDBOUND_API AWBTargetDummy : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AWBTargetDummy();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Wildbound|Dummy")
	TObjectPtr<UCapsuleComponent> CollisionCapsule;

	UPROPERTY(VisibleAnywhere, Category = "Wildbound|Dummy")
	TObjectPtr<UStaticMeshComponent> DisplayMesh;

	UPROPERTY(VisibleAnywhere, Category = "Wildbound|Dummy")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UWBAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "Wildbound|Dummy")
	TSubclassOf<UGameplayEffect> DefaultAttributesEffect;
};
