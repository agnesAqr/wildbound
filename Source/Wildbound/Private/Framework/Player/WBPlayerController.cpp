// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/Player/WBPlayerController.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Combat/WBAttributeSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/WBGameplayTags.h"
#include "Engine/World.h"
#include "Framework/Player/WBPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "Wildbound/Wildbound.h"

namespace WBMapPath
{
	static const TCHAR* Lobby = TEXT("/Game/Wildbound/Maps/L_Lobby");
	static const TCHAR* Raid  = TEXT("/Game/Wildbound/Maps/L_Raid");
}

static FName GetActiveMontageSection(const APawn* Pawn)
{
	const ACharacter* Character = Cast<ACharacter>(Pawn);
	const USkeletalMeshComponent* Mesh = Character ? Character->GetMesh() : nullptr;
	const UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
	if (!AnimInstance) return NAME_None;

	const UAnimMontage* ActiveMontage = AnimInstance->GetCurrentActiveMontage();
	return ActiveMontage ? AnimInstance->Montage_GetCurrentSection(ActiveMontage) : NAME_None;
}

void AWBPlayerController::WBTravelRaid()
{
	ServerTravelTo(WBMapPath::Raid);
}

void AWBPlayerController::WBTravelLobby()
{
	ServerTravelTo(WBMapPath::Lobby);
}

void AWBPlayerController::ServerTravelTo(const FString& MapPath)
{
	if (!HasAuthority())
	{
		UE_LOG(LogWildbound, Display,
			TEXT("[WB][PC][Travel] 클라이언트에서 호출 → 서버로 전달: %s"), *MapPath);
		Server_TravelTo(MapPath);
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogWildbound, Error, TEXT("[WB][PC][Travel] World가 없습니다."));
		return;
	}

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][PC][Travel] ServerTravel 시작 → %s"), *MapPath);

	World->ServerTravel(MapPath, /*bAbsolute=*/false);
}

void AWBPlayerController::Server_TravelTo_Implementation(const FString& MapPath)
{
	ServerTravelTo(MapPath);
}

void AWBPlayerController::WBSetProbe(int32 NewValue)
{
	if (!HasAuthority())
	{
		Server_SetProbe(NewValue);
		return;
	}

	Server_SetProbe_Implementation(NewValue);
}

void AWBPlayerController::Server_SetProbe_Implementation(int32 NewValue)
{
	AWBPlayerState* WBPlayerState = GetPlayerState<AWBPlayerState>();
	if (!WBPlayerState)
	{
		UE_LOG(LogWildbound, Error, TEXT("[WB][PC][Probe] PlayerState가 없습니다."));
		return;
	}

	WBPlayerState->SetTravelProbe(NewValue);

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][PC][Probe] 서버가 TravelProbe=%d 로 설정 (Player=%s)"),
		NewValue, *WBPlayerState->GetPlayerName());
}

void AWBPlayerController::WBShowProbe() const
{
	const AWBPlayerState* WBPlayerState = GetPlayerState<AWBPlayerState>();
	if (!WBPlayerState)
	{
		UE_LOG(LogWildbound, Error, TEXT("[WB][PC][Probe] PlayerState가 없습니다."));
		return;
	}

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][PC][Probe] 현재 TravelProbe=%d | Authority=%d | Player=%s"),
		WBPlayerState->GetTravelProbe(),
		HasAuthority() ? 1 : 0,
		*WBPlayerState->GetPlayerName());
}

void AWBPlayerController::WBShowAttributes() const
{
	const UWorld* World = GetWorld();
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	if (!GameState)
	{
		UE_LOG(LogWildbound, Error, TEXT("[WB][PC][Attr] GameState가 없습니다."));
		return;
	}

	const APlayerState* SelfPlayerState = GetPlayerState<APlayerState>();

	for (const APlayerState* EachPlayerState : GameState->PlayerArray)
	{
		const AWBPlayerState* WBPlayerState = Cast<AWBPlayerState>(EachPlayerState);
		if (!WBPlayerState) continue;

		const UWBAttributeSet* Attributes = WBPlayerState->GetAttributeSet();
		if (!Attributes) continue;

		const UAbilitySystemComponent* ASC = WBPlayerState->GetAbilitySystemComponent();
		const FString OwnedTags = ASC ? ASC->GetOwnedGameplayTags().ToStringSimple() : FString(TEXT("-"));

		UE_LOG(LogWildbound, Warning,
			TEXT("[WB][PC][Attr] Authority=%d | Target=%s | bSelf=%d | HP=%.0f/%.0f | SP=%.0f/%.0f | Section=%s | Tags=%s"),
			HasAuthority() ? 1 : 0,
			*WBPlayerState->GetPlayerName(),
			EachPlayerState == SelfPlayerState ? 1 : 0,
			Attributes->GetHealth(), Attributes->GetMaxHealth(),
			Attributes->GetStamina(), Attributes->GetMaxStamina(),
			*GetActiveMontageSection(WBPlayerState->GetPawn()).ToString(),
			*OwnedTags);
	}
}

void AWBPlayerController::WBGodMode()
{
	if (!HasAuthority())
	{
		Server_SetGodMode();
		return;
	}

	Server_SetGodMode_Implementation();
}

void AWBPlayerController::Server_SetGodMode_Implementation()
{
	AWBPlayerState* WBPlayerState = GetPlayerState<AWBPlayerState>();
	UAbilitySystemComponent* ASC =
		WBPlayerState ? WBPlayerState->GetAbilitySystemComponent() : nullptr;
	if (!ASC)
	{
		UE_LOG(LogWildbound, Error, TEXT("[WB][PC][God] PlayerState 또는 ASC가 없습니다."));
		return;
	}

	bGodMode = !bGodMode;

	if (bGodMode)
	{
		ASC->AddLooseGameplayTag(WBGameplayTags::State_Combat_Invulnerable);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(WBGameplayTags::State_Combat_Invulnerable);
	}

	UE_LOG(LogWildbound, Warning,
		TEXT("[WB][PC][God] %s | Player=%s | Count=%d"),
		bGodMode ? TEXT("ON") : TEXT("OFF"),
		*WBPlayerState->GetPlayerName(),
		ASC->GetGameplayTagCount(WBGameplayTags::State_Combat_Invulnerable));
}
