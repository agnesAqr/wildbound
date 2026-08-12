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
	/* Temporary exec commands for verification */

	UFUNCTION(Exec)
	void WBTravelRaid();

	UFUNCTION(Exec)
	void WBTravelLobby();

	UFUNCTION(Exec)
	void WBSetProbe(int32 NewValue);

	UFUNCTION(Exec)
	void WBShowProbe() const;

	UFUNCTION(Exec)
	void WBShowAttributes() const;

	UFUNCTION(Exec)
	void WBGodMode();

private:
	void ServerTravelTo(const FString& MapPath);

	UFUNCTION(Server, Reliable)
	void Server_TravelTo(const FString& MapPath);

	UFUNCTION(Server, Reliable)
	void Server_SetProbe(int32 NewValue);

	UFUNCTION(Server, Reliable)
	void Server_SetGodMode();

	bool bGodMode = false;
};
