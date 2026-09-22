#include "GameplayAbility_Stagger.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Data/CWGameplayTags.h"
#include "Interfaces/Combatant.h"
#include "Interfaces/Damageable.h"

UGameplayAbility_Stagger::UGameplayAbility_Stagger()
{
	SetAssetTags(CWGameplayTags::Ability_Stagger.GetTag().GetSingleTagContainer());
}

void UGameplayAbility_Stagger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!IsActive() || bIsAbilityEnding)
		return;

	auto LastHit = IDamageable::Execute_GetLastHitData(ActorInfo->AvatarActor.Get());
	if (LastHit.Causer && LastHit.Causer->Implements<UCombatant>())
		ICombatant::Execute_OnStaggeredActor(LastHit.Causer, ActorInfo->AvatarActor.Get(), LastHit);
	
	if (!bIsAbilityEnding)
	{
		if (ActorInfo->AvatarActor->Implements<UDamageable>())
			IDamageable::Execute_OnStaggerStarted(ActorInfo->AvatarActor.Get());
	}
}

void UGameplayAbility_Stagger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	HitReactMontageTask = nullptr;
	if (ActorInfo->AvatarActor->Implements<UDamageable>())
		IDamageable::Execute_OnStaggerFinished(ActorInfo->AvatarActor.Get());
}

void UGameplayAbility_Stagger::PlayMontage_Implementation(UAnimMontage* Montage)
{
	HitReactMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Stagger"), Montage);
	HitReactMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnHitReactMontageCompleted);
	HitReactMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnHitReactMontageInterrupted);
	HitReactMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnHitReactMontageCancelled);
	HitReactMontageTask->ReadyForActivation();
}

void UGameplayAbility_Stagger::OnHitReactMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGameplayAbility_Stagger::OnHitReactMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_Stagger::OnHitReactMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
