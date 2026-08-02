// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WBGameStateBase.generated.h"

UCLASS()
class WILDBOUND_API AWBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	/**
	 * 접속 인원은 복제 상태이므로 GameState가 보고한다.
	 * PlayerArray가 실제로 바뀐 직후에 세야 정확하다 — AGameModeBase::Logout 시점에는
	 * 나가는 PlayerState가 아직 배열에 남아 있어 1이 크게 나온다.
	 * 서버뿐 아니라 각 클라이언트에서도 호출되므로 로그에 실행 컨텍스트를 함께 남긴다.
	 */
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

protected:
	virtual void BeginPlay() override;

private:
	void LogPlayerRoster(const TCHAR* Event, const APlayerState* PlayerState) const;
};
