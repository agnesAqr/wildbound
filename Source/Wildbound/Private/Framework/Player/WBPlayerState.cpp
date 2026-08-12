// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/Player/WBPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Combat/WBAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Wildbound/Wildbound.h"

AWBPlayerState::AWBPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Must be a subobject of the same actor as the ASC, or InitializeComponent will not auto-register it into SpawnedAttributes.
	AttributeSet = CreateDefaultSubobject<UWBAttributeSet>(TEXT("AttributeSet"));

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

void AWBPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][PlayerState][BeginPlay] Player=%s | Role=%d | TravelProbe=%d"),
		*GetPlayerName(), static_cast<int32>(GetLocalRole()), TravelProbe);
}

void AWBPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);

	if (AWBPlayerState* NewWBPlayerState = Cast<AWBPlayerState>(NewPlayerState))
	{
		NewWBPlayerState->TravelProbe = TravelProbe;

		UE_LOG(LogWildbound, Warning,
			TEXT("[WB][PlayerState][CopyProperties] Player=%s | TravelProbe %d 이전됨"),
			*GetPlayerName(), TravelProbe);
	}
	else
	{
		UE_LOG(LogWildbound, Error,
			TEXT("[WB][PlayerState][CopyProperties] 대상이 AWBPlayerState가 아님 — 값 이전 실패"));
	}
}

void AWBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWBPlayerState, TravelProbe);
}
