#include "GameplayAbility_Death.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Data/CWGameplayTags.h"
#include "Interfaces/Combatant.h"
#include "Interfaces/Damageable.h"

UGameplayAbility_Death::UGameplayAbility_Death()
{
	SetAssetTags(CWGameplayTags::Ability_Death.GetTag().GetSingleTagContainer());
}

bool UGameplayAbility_Death::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// Death must activate at zero health; bypass HitReact's alive check.
	return UGameplayAbility::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)
		&& ActorInfo && ActorInfo->AvatarActor.IsValid() && ActorInfo->AvatarActor->Implements<UDamageable>();
}

void UGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!IsActive() || bIsAbilityEnding)
		return;
	
	auto AvatarActor = ActorInfo->AvatarActor.Get();
	IDamageable::Execute_OnDeathStarted(AvatarActor);
	auto LastHit = IDamageable::Execute_GetLastHitData(AvatarActor);
	if (IsValid(LastHit.Causer) && LastHit.Causer->Implements<UCombatant>())
		ICombatant::Execute_OnKilledActor(LastHit.Causer, AvatarActor, LastHit);
	
	WaitFinishDeathEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CWGameplayTags::Ability_Death);
	WaitFinishDeathEvent->EventReceived.AddDynamic(this, &UGameplayAbility_Death::OnDeathFinishedExternalEvent);
	WaitFinishDeathEvent->ReadyForActivation();
}

void UGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	HitReactMontageTask = nullptr;
	if (ActorInfo->AvatarActor.IsValid() && ActorInfo->AvatarActor->Implements<UDamageable>())
		IDamageable::Execute_OnDeathFinished(ActorInfo->AvatarActor.Get());
}

void UGameplayAbility_Death::OnDeathFinishedExternalEvent_Implementation(FGameplayEventData Payload)
{
	if (IsEndAbilityValid(CurrentSpecHandle, CurrentActorInfo))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, GetCurrentActivationInfo(), true, false);
}

void UGameplayAbility_Death::PlayMontage_Implementation(UAnimMontage* Montage)
{
	HitReactMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Death"), Montage);
	HitReactMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnHitReactMontageCompleted);
	HitReactMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnHitReactMontageInterrupted);
	HitReactMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnHitReactMontageCancelled);
	HitReactMontageTask->ReadyForActivation();
}

void UGameplayAbility_Death::OnHitReactMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGameplayAbility_Death::OnHitReactMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_Death::OnHitReactMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
