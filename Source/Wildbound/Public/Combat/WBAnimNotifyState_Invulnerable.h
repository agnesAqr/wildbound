// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CoreMinimal.h"
#include "WBAnimNotifyState_Invulnerable.generated.h"

UCLASS(meta = (DisplayName = "WB Invulnerable"))
class WILDBOUND_API UWBAnimNotifyState_Invulnerable : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

private:
	void SetInvulnerable(USkeletalMeshComponent* MeshComp, bool bEnable) const;
};
