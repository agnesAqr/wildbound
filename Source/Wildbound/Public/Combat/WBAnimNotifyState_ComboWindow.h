// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CoreMinimal.h"
#include "WBAnimNotifyState_ComboWindow.generated.h"

struct FGameplayTag;

UCLASS(meta = (DisplayName = "WB Combo Window"))
class WILDBOUND_API UWBAnimNotifyState_ComboWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

private:
	void SendComboWindowEvent(USkeletalMeshComponent* MeshComp, const FGameplayTag& EventTag) const;
};
