// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WBPlayerController.generated.h"

UCLASS()
class WILDBOUND_API AWBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/* 검증용 임시 exec 커맨드 */

	UFUNCTION(Exec)
	void WBTravelRaid();

	UFUNCTION(Exec)
	void WBTravelLobby();

	UFUNCTION(Exec)
	void WBSetProbe(int32 NewValue);

	UFUNCTION(Exec)
	void WBShowProbe() const;

private:
	void ServerTravelTo(const FString& MapPath);

	UFUNCTION(Server, Reliable)
	void Server_TravelTo(const FString& MapPath);

	UFUNCTION(Server, Reliable)
	void Server_SetProbe(int32 NewValue);
};
