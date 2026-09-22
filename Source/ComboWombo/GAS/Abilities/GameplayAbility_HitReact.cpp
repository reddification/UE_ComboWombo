#include "GameplayAbility_HitReact.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Chooser.h"
#include "GameplayEffect.h"
#include "Characters/CWCharacterBase.h"
#include "Data/HitReactChooserParameters.h"
#include "Interfaces/Damageable.h"

UGameplayAbility_HitReact::UGameplayAbility_HitReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!IsActive() || bIsAbilityEnding)
		return;
	
	AActor* AvatarActor = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
	if (!IsValid(AvatarActor) || !AvatarActor->Implements<UDamageable>())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FReceivedHitData LastHit = IDamageable::Execute_GetLastHitData(AvatarActor);
	if (ActiveEffect)
	{
		const UGameplayEffect* Effect = ActiveEffect->GetDefaultObject<UGameplayEffect>();
		if (ensureMsgf(Effect->DurationPolicy == EGameplayEffectDurationType::Infinite
			&& Effect->GetStackingType() == EGameplayEffectStackingType::None,
			TEXT("ActiveEffect must be infinite and non-stacking to follow the reaction ability lifetime.")))
		{
			ActiveEffectHandle = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, Effect, GetAbilityLevel());
		}
	}

	UAnimMontage* Montage = nullptr;
	if (MontageChooser)
	{
		FHitReactChooserParameters Parameters;
		Parameters.HitType = LastHit.HitType.GetSingleTagContainer();
		Parameters.BodyRegion = LastHit.BodyRegion.GetSingleTagContainer();
		Parameters.ImpactDirection = LastHit.ImpactDirectionTag.GetSingleTagContainer();
		FChooserEvaluationContext Context;
		Context.AddStructParam(Parameters);
		FEvaluateChooser Chooser(MontageChooser);
		Montage = Cast<UAnimMontage>(Chooser.ChooseObject(Context));
	}
	
	if (!Montage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	PlayMontage(Montage);
}

void UGameplayAbility_HitReact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActiveEffectHandle.IsValid() && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
		ActiveEffectHandle.Invalidate();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGameplayAbility_HitReact::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;
	
	const ACWCharacterBase* Owner = ActorInfo ? Cast<ACWCharacterBase>(ActorInfo->AvatarActor.Get()) : nullptr;
	return Owner && !Owner->IsDead();
}

void UGameplayAbility_HitReact::PlayMontage_Implementation(UAnimMontage* Montage)
{
}
