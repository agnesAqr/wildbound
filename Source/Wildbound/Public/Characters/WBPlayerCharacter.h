// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WBCharacterBase.h"
#include "GameplayTagContainer.h"
#include "WBPlayerCharacter.generated.h"

class UCameraComponent;
class UGameplayEffect;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
struct FInputActionValue;

/** Maps an input action to the ability tag it activates. */
USTRUCT(BlueprintType)
struct FWBAbilityInputBinding
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (Categories = "Ability"))
	FGameplayTag AbilityTag;
};

UCLASS()
class WILDBOUND_API AWBPlayerCharacter : public AWBCharacterBase
{
	GENERATED_BODY()

public:
	AWBPlayerCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void NotifyControllerChanged() override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void InitAbilityActorInfoFromPlayerState();

	bool IsLocomotionLocked() const;

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Jump();
	void Input_AbilityInputPressed(FGameplayTag AbilityTag);
	void Input_AbilityInputReleased(FGameplayTag AbilityTag);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> StaminaRegenEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TArray<FWBAbilityInputBinding> AbilityInputBindings;
};
