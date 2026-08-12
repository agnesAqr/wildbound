// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/WBCharacterBase.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "Combat/WBGameplayAbility.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/WBGameplayTags.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameplayEffect.h"
#include "Wildbound/Wildbound.h"

static TAutoConsoleVariable<int32> CVarWBShowMeleeTrace(
	TEXT("wb.ShowMeleeTrace"), 0,
	TEXT("1이면 근접 트레이스 구간을 그린다 (server=red, client=green)"),
	ECVF_Cheat);

AWBCharacterBase::AWBCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

UAbilitySystemComponent* AWBCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AWBCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& EffectClass)
{
	if (!HasAuthority() || !AbilitySystemComponent || !EffectClass) return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle =
		AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.0f, EffectContext);
	if (!SpecHandle.IsValid()) return;

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AWBCharacterBase::InitializeDefaultAttributes()
{
	ApplyEffectToSelf(DefaultAttributesEffect);
}

void AWBCharacterBase::GiveDefaultAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	for (const TSubclassOf<UWBGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if (!AbilityClass || AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass)) continue;

		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
	}
}

void AWBCharacterBase::BeginMeleeTrace(USkeletalMeshComponent* MeshComp, float InTraceRadius, int32 InSampleCount)
{
	if (!MeshComp || MeleeSocketPairs.Num() == 0) return;

	ActiveTraceRadius = InTraceRadius;
	ActiveSampleCount = FMath::Max(InSampleCount, 2);
	MeleeHitActors.Reset();

	GatherTraceSamples(MeshComp, PreviousTraceSamples);
}

void AWBCharacterBase::TickMeleeTrace(USkeletalMeshComponent* MeshComp)
{
	if (!MeshComp || PreviousTraceSamples.Num() == 0) return;

	TArray<FVector> CurrentSamples;
	GatherTraceSamples(MeshComp, CurrentSamples);
	if (CurrentSamples.Num() != PreviousTraceSamples.Num()) return;

	if (HasAuthority())
	{
		const FCollisionObjectQueryParams ObjectParams(ECC_Pawn);
		const FCollisionQueryParams QueryParams(TEXT("WBMeleeTrace"), false, this);

		for (int32 Index = 0; Index < CurrentSamples.Num(); ++Index)
		{
			TArray<FHitResult> Hits;
			GetWorld()->SweepMultiByObjectType(
				Hits,
				PreviousTraceSamples[Index],
				CurrentSamples[Index],
				FQuat::Identity,
				ObjectParams,
				FCollisionShape::MakeSphere(ActiveTraceRadius), QueryParams);

			for (const FHitResult& Hit : Hits)
			{
				AActor* HitActor = Hit.GetActor();
				if (!HitActor || MeleeHitActors.Contains(HitActor)) continue;

				MeleeHitActors.Add(HitActor);
				HandleMeleeHit(HitActor, Hit);
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	if (CVarWBShowMeleeTrace.GetValueOnAnyThread() > 0)
	{
		const FColor DebugColor = HasAuthority() ? FColor::Red : FColor::Green;

		for (int32 Index = 0; Index < CurrentSamples.Num(); ++Index)
		{
			DrawDebugLine(GetWorld(), PreviousTraceSamples[Index], CurrentSamples[Index],
				DebugColor, false, 1.0f, 0, 1.0f);
		}
	}
#endif

	PreviousTraceSamples = MoveTemp(CurrentSamples);
}

void AWBCharacterBase::EndMeleeTrace()
{
	PreviousTraceSamples.Reset();
	MeleeHitActors.Reset();
	ActiveTraceRadius = 0.0f;
	ActiveSampleCount = 0;
}

void AWBCharacterBase::GatherTraceSamples(USkeletalMeshComponent* MeshComp, TArray<FVector>& OutSamples) const
{
	OutSamples.Reset(MeleeSocketPairs.Num() * ActiveSampleCount);

	for (const FWBMeleeSocketPair& Pair : MeleeSocketPairs)
	{
		const FVector Start = MeshComp->GetSocketLocation(Pair.StartSocket);
		const FVector End = MeshComp->GetSocketLocation(Pair.EndSocket);

		for (int32 Index = 0; Index < ActiveSampleCount; ++Index)
		{
			const float Alpha = static_cast<float>(Index) / static_cast<float>(ActiveSampleCount - 1);
			OutSamples.Add(FMath::Lerp(Start, End, Alpha));
		}
	}
}

void AWBCharacterBase::HandleMeleeHit(AActor* HitActor, const FHitResult& Hit)
{
	UE_LOG(LogWildbound, Display,
		TEXT("[WB][Melee][Hit] IsServer=%d | Attacker=%s | Target=%s"),
		HasAuthority() ? 1 : 0, *GetName(), *GetNameSafe(HitActor));

	if (!AbilitySystemComponent) return;

	FGameplayEventData Payload;
	Payload.EventTag = WBGameplayTags::Event_Hit_Melee;
	Payload.Instigator = this;
	Payload.Target = HitActor;

	AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
}
