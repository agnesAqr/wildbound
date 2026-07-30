// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/Player/WBPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

AWBPlayerState::AWBPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	SetNetUpdateFrequency(100.0f);
}

UAbilitySystemComponent* AWBPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AWBPlayerState::SetTravelProbe(int32 InValue)
{
	if (!HasAuthority()) return;

	if (TravelProbe != InValue)
	{
		TravelProbe = InValue;
		ForceNetUpdate();
	}
}

void AWBPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);

	if (AWBPlayerState* NewWBPlayerState = Cast<AWBPlayerState>(NewPlayerState))
	{
		NewWBPlayerState->TravelProbe = TravelProbe;
	}
}

void AWBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWBPlayerState, TravelProbe);
}
