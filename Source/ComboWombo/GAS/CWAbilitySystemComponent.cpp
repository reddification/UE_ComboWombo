#include "GAS/CWAbilitySystemComponent.h"

#include "Data/AbilitySet.h"
#include "Data/CWGameplayTags.h"
#include "GAS/Attributes/MovementAttributeSet.h"
#include "GAS/Attributes/VitalsAttributeSet.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCWAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	if (IsTemplate() || (GetOwner() && GetOwner()->IsTemplate()))
		return;
}

void UCWAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto Character = Cast<ACharacter>(GetAvatarActor()))
		CMC = Character->GetCharacterMovement();
	
	BindAttributeDelegates();

	FOnAttributeChangeData InitialMovementSpeed;
	InitialMovementSpeed.NewValue = GetNumericAttribute(UMovementAttributeSet::GetMovementSpeedAttribute());
	OnMovementSpeedChanged(InitialMovementSpeed);
	
	FOnAttributeChangeData InitialTurnRate;
	InitialMovementSpeed.NewValue = GetNumericAttribute(UMovementAttributeSet::GetTurnRateAttribute());
	OnTurnRateChanged(InitialTurnRate);
	
	if (!AbilitySet.IsNull())
	{
		if (AbilitySet.IsValid())
		{
			AbilitySet->GiveToAbilitySystem(this, &GrantedAbilitySet);
		}
		else
		{
			AbilitySet.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateWeakLambda(this, [this](const FSoftObjectPath& Path, 
				UObject* LoadedAbilitySetRaw)
			{
				if (!LoadedAbilitySetRaw)
					return;
					
				StaticCast<UAbilitySet*>(LoadedAbilitySetRaw)->GiveToAbilitySystem(this, &GrantedAbilitySet);
			}));
		}
	}
}

void UCWAbilitySystemComponent::OutOfPoise_Implementation()
{
	TryActivateAbilitiesByTag(CWGameplayTags::Ability_Stagger.GetTag().GetSingleTagContainer());
}

// one could argue that handling all attributes changes in a single component is breaking (S)OLID principle,
// but I guess it'd do for a test assignment
// alternative is having a separate proxy component for each attribute set
void UCWAbilitySystemComponent::BindAttributeDelegates()
{
	if (!HealthChangedDelegateHandle.IsValid())
	{
		HealthChangedDelegateHandle = GetGameplayAttributeValueChangeDelegate(UVitalsAttributeSet::GetHealthAttribute())
			.AddUObject(this, &ThisClass::OnHealthChanged);
	}

	if (!MovementSpeedChangedDelegateHandle.IsValid())
	{
		MovementSpeedChangedDelegateHandle = GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GetMovementSpeedAttribute())
			.AddUObject(this, &ThisClass::OnMovementSpeedChanged);
	}
	
	if (!TurnRateChangedDelegateHandle.IsValid())
	{
		TurnRateChangedDelegateHandle = GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GetTurnRateAttribute())
			.AddUObject(this, &ThisClass::OnTurnRateChanged);
	}
	
	if (IsValid(RestorePoiseEffect))
		GetGameplayAttributeValueChangeDelegate(UVitalsAttributeSet::GetPoiseAttribute()).AddUObject(this, &UCWAbilitySystemComponent::OnPoiseChanged);

}

void UCWAbilitySystemComponent::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.NewValue <= 0.0f)
		StartDeath();
}

void UCWAbilitySystemComponent::OnMovementSpeedChanged(const FOnAttributeChangeData& ChangeData)
{
	if (CMC)
		CMC->MaxWalkSpeed = FMath::Max(0.0f, ChangeData.NewValue);
}

void UCWAbilitySystemComponent::OnTurnRateChanged(const FOnAttributeChangeData& ChangeData)
{
	if (CMC)
		CMC->RotationRate.Yaw = FMath::Max(0.f, ChangeData.NewValue);
}

void UCWAbilitySystemComponent::OnPoiseChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (OnAttributeChangeData.OldValue > 0.f && OnAttributeChangeData.NewValue <= 0.f)
		OutOfPoise();
		
	auto MaxPoise = GetNumericAttribute(UVitalsAttributeSet::GetMaxPoiseAttribute());
	OnRestorableAttributeChanged(OnAttributeChangeData.NewValue, OnAttributeChangeData.OldValue, MaxPoise, StartRestorePoiseTimer,
		RestorePoiseEffectStartDelay, RestorePoiseEffect, ActiveRestorePoiseEffectSpec);
}

void UCWAbilitySystemComponent::OnRestorableAttributeChanged(float NewValue, float OldValue, float MaxValue, FTimerHandle& RestoreStartTimer,
	float RestoreEffectActivationDelay, const TSubclassOf<UGameplayEffect>& RestoreEffect, FActiveGameplayEffectHandle& ActiveRestoreAttributeEffectSpec)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (NewValue >= MaxValue && ActiveRestoreAttributeEffectSpec.IsValid())
	{
		RemoveActiveGameplayEffect(ActiveRestoreAttributeEffectSpec);
		ActiveRestoreAttributeEffectSpec.Invalidate();
	}
	else if (NewValue < OldValue)
	{
		if (ActiveRestoreAttributeEffectSpec.IsValid())
		{
			RemoveActiveGameplayEffect(ActiveRestoreAttributeEffectSpec);
			ActiveRestoreAttributeEffectSpec.Invalidate();
		}
		
		if (TimerManager.IsTimerActive(RestoreStartTimer))
			TimerManager.ClearTimer(RestoreStartTimer);

		auto TimerDelegate = FTimerDelegate::CreateUObject(this, &UCWAbilitySystemComponent::StartRestoreAttribute,
			&RestoreEffect, &ActiveRestoreAttributeEffectSpec);
		TimerManager.SetTimer(RestoreStartTimer, TimerDelegate, RestoreEffectActivationDelay, false);
	}
}

void UCWAbilitySystemComponent::StartRestoreAttribute(const TSubclassOf<UGameplayEffect>* RestoreGameplayEffect,
                                                      FActiveGameplayEffectHandle* ActiveRestoreEffectSpec)
{
	auto EffectContext = MakeEffectContext();
	auto EffectSpec = MakeOutgoingSpec(*RestoreGameplayEffect, 1.f, EffectContext);
	*ActiveRestoreEffectSpec = ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
}

void UCWAbilitySystemComponent::StartDeath_Implementation()
{
	TryActivateAbilitiesByTag(CWGameplayTags::Ability_Death.GetTag().GetSingleTagContainer());
}
