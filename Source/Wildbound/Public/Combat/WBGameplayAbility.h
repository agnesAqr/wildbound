// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WBGameplayAbility.generated.h"

/**
 * 프로젝트 전 어빌리티의 공통 베이스. 인스턴싱·네트워크 실행 정책의 기본값을 한 곳에 고정한다.
 */
UCLASS(Abstract)
class WILDBOUND_API UWBGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UWBGameplayAbility();
};
