// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/WBPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Core/WBGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Player/WBPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Wildbound/Wildbound.h"

AWBPlayerCharacter::AWBPlayerCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.0f, 250.0f, 0.0f);
	}

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	FWBMeleeSocketPair KwangGreatsword;
	KwangGreatsword.StartSocket = TEXT("FX_weapon_base");
	KwangGreatsword.EndSocket = TEXT("FX_weapon_tip");
	MeleeSocketPairs.Add(KwangGreatsword);
}

void AWBPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfoFromPlayerState();  // Server
}

void AWBPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfoFromPlayerState();  // Client
}

void AWBPlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !DefaultMappingContext) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
}

void AWBPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogWildbound, Error,
			TEXT("[WB][PlayerCharacter][Input] EnhancedInputComponent가 아님 — DefaultInputComponentClass 확인 필요"));
		return;
	}

	if (MoveAction)
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWBPlayerCharacter::Input_Move);
	}

	if (LookAction)
	{
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWBPlayerCharacter::Input_Look);
	}

	if (JumpAction)
	{
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}

	for (const FWBAbilityInputBinding& Binding : AbilityInputBindings)
	{
		if (!Binding.InputAction || !Binding.AbilityTag.IsValid()) continue;

		EnhancedInput->BindAction(Binding.InputAction, ETriggerEvent::Started, this,
			&AWBPlayerCharacter::Input_AbilityInputPressed, Binding.AbilityTag);

		EnhancedInput->BindAction(Binding.InputAction, ETriggerEvent::Completed, this,
			&AWBPlayerCharacter::Input_AbilityInputReleased, Binding.AbilityTag);
	}
}

void AWBPlayerCharacter::InitAbilityActorInfoFromPlayerState()
{
	AWBPlayerState* WBPlayerState = GetPlayerState<AWBPlayerState>();
	if (!WBPlayerState) return;

	AbilitySystemComponent = WBPlayerState->GetAbilitySystemComponent();
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogWildbound, Error,
			TEXT("[WB][PlayerCharacter][InitASC] PlayerState에 ASC가 없음 | HasAuthority=%d"),
			HasAuthority() ? 1 : 0);
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(WBPlayerState, this);

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][PlayerCharacter][InitASC] HasAuthority=%d | Player=%s | Avatar=%s"),
		HasAuthority() ? 1 : 0, *WBPlayerState->GetPlayerName(), *GetName());

	InitializeDefaultAttributes();
	GiveDefaultAbilities();
}

void AWBPlayerCharacter::Input_Move(const FInputActionValue& Value)
{
	const AController* MyController = GetController();
	if (!MyController) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator YawRotation(0.0f, MyController->GetControlRotation().Yaw, 0.0f);
	const FRotationMatrix YawMatrix(YawRotation);

	AddMovementInput(YawMatrix.GetUnitAxis(EAxis::X), MovementVector.Y);
	AddMovementInput(YawMatrix.GetUnitAxis(EAxis::Y), MovementVector.X);
}

void AWBPlayerCharacter::Input_Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AWBPlayerCharacter::Input_AbilityInputPressed(FGameplayTag AbilityTag)
{
	if (!AbilitySystemComponent) return;

	UE_LOG(LogWildbound, Display,
		TEXT("[WB][Input][Pressed] Tag=%s | Attacking=%d"),
		*AbilityTag.GetTagName().ToString(),
		AbilitySystemComponent->HasMatchingGameplayTag(WBGameplayTags::State_Combat_Attacking) ? 1 : 0);

	if (AbilityTag == WBGameplayTags::Ability_Attack_Primary &&
		AbilitySystemComponent->HasMatchingGameplayTag(WBGameplayTags::State_Combat_Attacking))
	{
		FGameplayEventData Payload;
		Payload.EventTag = WBGameplayTags::Event_Input_AttackPrimary;
		Payload.Instigator = this;

		AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		return;
	}

	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag));
}

void AWBPlayerCharacter::Input_AbilityInputReleased(FGameplayTag AbilityTag)
{
	if (!AbilitySystemComponent) return;

	TArray<FGameplayAbilitySpecHandle> Handles;
	AbilitySystemComponent->FindAllAbilitiesWithTags(Handles, FGameplayTagContainer(AbilityTag));

	for (const FGameplayAbilitySpecHandle& Handle : Handles)
	{
		if (FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle))
		{
			AbilitySystemComponent->AbilitySpecInputReleased(*Spec);
		}
	}
}
