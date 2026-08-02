// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "WBPlayerState.generated.h"

class UAbilitySystemComponent;

UCLASS()
class WILDBOUND_API AWBPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AWBPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	int32 GetTravelProbe() const { return TravelProbe; }
	void SetTravelProbe(int32 InValue);

protected:
	virtual void BeginPlay() override;
	virtual void CopyProperties(APlayerState* NewPlayerState) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// Test
	UPROPERTY(Replicated)
	int32 TravelProbe = 0;
};
