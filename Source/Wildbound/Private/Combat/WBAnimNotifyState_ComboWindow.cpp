// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/WBAnimNotifyState_ComboWindow.h"

#include "AbilitySystemComponent.h"
#include "Characters/WBCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/WBGameplayTags.h"
#include "Wildbound/Wildbound.h"

void UWBAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	SendComboWindowEvent(MeshComp, WBGameplayTags::Event_Combo_WindowBegin);
}

void UWBAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	SendComboWindowEvent(MeshComp, WBGameplayTags::Event_Combo_WindowEnd);
}

void UWBAnimNotifyState_ComboWindow::SendComboWindowEvent(USkeletalMeshComponent* MeshComp,
	const FGameplayTag& EventTag) const
{
	AWBCharacterBase* Character = MeshComp ? Cast<AWBCharacterBase>(MeshComp->GetOwner()) : nullptr;
	UAbilitySystemComponent* AbilitySystemComponent =
		Character ? Character->GetAbilitySystemComponent() : nullptr;
	if (!AbilitySystemComponent) return;

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][Combo][Window] %s | IsServer=%d"),
		*EventTag.GetTagName().ToString(), Character->HasAuthority() ? 1 : 0);

	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = Character;

	AbilitySystemComponent->HandleGameplayEvent(EventTag, &Payload);
}
