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
	virtual void CopyProperties(APlayerState* NewPlayerState) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/** W2 기준 ⑤ 검증용. Travel 후에도 값이 남으면 Seamless Travel과 CopyProperties가 동작한 것이다. */
	UPROPERTY(Replicated)
	int32 TravelProbe = 0;
};
