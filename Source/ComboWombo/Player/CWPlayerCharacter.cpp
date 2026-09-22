// Copyright Epic Games, Inc. All Rights Reserved.

#include "CWPlayerCharacter.h"
#include "GAS/CWAbilitySystemComponent.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ComboWombo.h"
#include "Components/Player/PlayerPerceptionComponent.h"
#include "Components/Player/PlayerTargetingComponent.h"
#include "Data/CWGameplayTags.h"
#include "Engine/World.h"
#include "Settings/CombatSettings.h"
#include "Subsystems/HitStopSubsystem.h"

ACWPlayerCharacter::ACWPlayerCharacter()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	SetCameraControlledStrafing(true);
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	PlayerPerceptionComponent = CreateDefaultSubobject<UPlayerPerceptionComponent>(TEXT("PlayerPerceptionComponent"));
	PlayerTargetingComponent = CreateDefaultSubobject<UPlayerTargetingComponent>(TEXT("PlayerTargetingComponent"));
}

void ACWPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateSprintState();
}

void ACWPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACWPlayerCharacter::OnJumpInput);
		// EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACWPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACWPlayerCharacter::ClearCachedMovementInput);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ACWPlayerCharacter::ClearCachedMovementInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACWPlayerCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACWPlayerCharacter::Look);
		
		if (AttackAction)
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACWPlayerCharacter::OnAttackInput);
		
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACWPlayerCharacter::OnRequestSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACWPlayerCharacter::OnStopSprint);
		}
	}
	else
	{
		UE_LOG(LogComboWombo, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACWPlayerCharacter::UnPossessed()
{
	ClearCachedMovementInput();
	Super::UnPossessed();
}

void ACWPlayerCharacter::ClearCachedMovementInput()
{
	CachedMovementInputDirection = FVector::ZeroVector;
}

FVector ACWPlayerCharacter::GetAttackInputDirection() const
{
	return CachedMovementInputDirection.IsNearlyZero() ? GetActorForwardVector() : CachedMovementInputDirection;
}

void ACWPlayerCharacter::UpdateSprintState_Implementation()
{
	if (bWantsToSprint && !IsSprinting() && GetVelocity().SizeSquared2D() > 0)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(CWGameplayTags::Ability_Sprint.GetTag().GetSingleTagContainer());
	}
	else if ((!bWantsToSprint || FMath::IsNearlyZero(GetVelocity().SizeSquared2D())) && IsSprinting())
	{
		FGameplayTagContainer AbilityTags = CWGameplayTags::Ability_Sprint.GetTag().GetSingleTagContainer();
		AbilitySystemComponent->CancelAbilities(&AbilityTags);
	}
}

bool ACWPlayerCharacter::IsSprinting_Implementation() const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(CWGameplayTags::State_Sprinting);
}

void ACWPlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ACWPlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ACWPlayerCharacter::DoMove(float Right, float Forward)
{
	ClearCachedMovementInput();

	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Cache intent independently of whether the movement component accepts input.
		CachedMovementInputDirection = (ForwardDirection * Forward + RightDirection * Right).GetSafeNormal();

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ACWPlayerCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ACWPlayerCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ACWPlayerCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ACWPlayerCharacter::OnJumpInput_Implementation()
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(CWGameplayTags::Ability_Jump.GetTag().GetSingleTagContainer());
}

TArray<AActor*> ACWPlayerCharacter::GetEnemies_Implementation() const
{
	return PlayerPerceptionComponent->GetObservedActors();
}

void ACWPlayerCharacter::OnKilledActor_Implementation(AActor* KilledActor, const FReceivedHitData& LastHit)
{
	Super::OnKilledActor_Implementation(KilledActor, LastHit);
	if (!IsPlayerControlled() || !IsLocallyControlled() || !IsValid(KilledActor) || KilledActor == this)
		return;

	const UCombatSettings* Settings = GetDefault<UCombatSettings>();
	if (UHitStopSubsystem* HitStop = GetWorld()->GetSubsystem<UHitStopSubsystem>())
		HitStop->TryHitStop(Settings->DeathHitStopChance, Settings->DeathHitStopDuration);
}

void ACWPlayerCharacter::OnStaggeredActor_Implementation(AActor* StaggeredActor, const FReceivedHitData& LastHit)
{
	Super::OnStaggeredActor_Implementation(StaggeredActor, LastHit);
	if (!IsPlayerControlled() || !IsLocallyControlled() || !IsValid(StaggeredActor) || StaggeredActor == this)
		return;

	const UCombatSettings* Settings = GetDefault<UCombatSettings>();
	if (UHitStopSubsystem* HitStop = GetWorld()->GetSubsystem<UHitStopSubsystem>())
		HitStop->TryHitStop(Settings->StaggerHitStopChance, Settings->StaggerHitStopDuration);
}

AActor* ACWPlayerCharacter::GetBestTarget_Implementation() const
{
	return PlayerTargetingComponent->FindBestTarget(PlayerPerceptionComponent->GetObservedActors(), GetAttackInputDirection());
}

void ACWPlayerCharacter::OnAttackStarted_Implementation()
{
	Super::OnAttackStarted_Implementation();
	SetAttackRotationControl(true);
}

void ACWPlayerCharacter::OnAttackEnded_Implementation()
{
	Super::OnAttackEnded_Implementation();
	SetAttackRotationControl(false);
}

void ACWPlayerCharacter::OnRequestSprint()
{
	bWantsToSprint = true;
}

void ACWPlayerCharacter::OnStopSprint()
{
	bWantsToSprint = false;
}

void ACWPlayerCharacter::OnAttackInput_Implementation()
{
	if (ICombatant::Execute_IsAttacking(this))
	{
		FGameplayEventData GameplayEventData;
		AbilitySystemComponent->HandleGameplayEvent(CWGameplayTags::Ability_Attack_Event_Combo_Request, &GameplayEventData);
	}
	else
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(CWGameplayTags::Ability_Attack.GetTag().GetSingleTagContainer());
	}
}

void ACWPlayerCharacter::SetCameraControlledStrafing(bool bEnabled)
{
	bCameraControlledStrafing = bEnabled;
	UpdateRotationControl();
}
