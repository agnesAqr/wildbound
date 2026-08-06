// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBTargetDummy.h"

#include "AbilitySystemComponent.h"
#include "Combat/WBAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameplayEffect.h"
#include "Wildbound/Wildbound.h"

AWBTargetDummy::AWBTargetDummy()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(false);

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	CollisionCapsule->InitCapsuleSize(45.0f, 95.0f);
	CollisionCapsule->SetCollisionProfileName(TEXT("Pawn"));
	SetRootComponent(CollisionCapsule);

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(CollisionCapsule);
	DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UWBAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AWBTargetDummy::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AWBTargetDummy::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority() || !DefaultAttributesEffect) return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle =
		AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributesEffect, 1.0f, EffectContext);
	if (!SpecHandle.IsValid()) return;

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][Dummy][Init] %s | HP=%.1f/%.1f"),
		*GetName(), AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
}
