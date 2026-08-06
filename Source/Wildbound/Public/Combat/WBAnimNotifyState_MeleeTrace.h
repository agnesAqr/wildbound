// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CoreMinimal.h"
#include "WBAnimNotifyState_MeleeTrace.generated.h"

UCLASS(meta = (DisplayName = "WB Melee Trace"))
class WILDBOUND_API UWBAnimNotifyState_MeleeTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Wildbound|Melee", meta = (ClampMin = "1.0"))
	float TraceRadius = 12.0f;

	UPROPERTY(EditAnywhere, Category = "Wildbound|Melee", meta = (ClampMin = "2", ClampMax = "16"))
	int32 SampleCount = 5;
};
