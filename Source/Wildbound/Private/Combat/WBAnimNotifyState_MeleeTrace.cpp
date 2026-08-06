// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBAnimNotifyState_MeleeTrace.h"

#include "Characters/WBCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"

void UWBAnimNotifyState_MeleeTrace::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AWBCharacterBase* Character = MeshComp ? Cast<AWBCharacterBase>(MeshComp->GetOwner()) : nullptr)
	{
		Character->BeginMeleeTrace(MeshComp, TraceRadius, SampleCount);
	}
}

void UWBAnimNotifyState_MeleeTrace::NotifyTick(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (AWBCharacterBase* Character = MeshComp ? Cast<AWBCharacterBase>(MeshComp->GetOwner()) : nullptr)
	{
		Character->TickMeleeTrace(MeshComp);
	}
}

void UWBAnimNotifyState_MeleeTrace::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AWBCharacterBase* Character = MeshComp ? Cast<AWBCharacterBase>(MeshComp->GetOwner()) : nullptr)
	{
		Character->EndMeleeTrace();
	}
}
