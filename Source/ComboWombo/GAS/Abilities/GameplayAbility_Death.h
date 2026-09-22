#pragma once

#include "GameplayAbility_HitReact.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayAbility_Death.generated.h"

UCLASS()
class COMBOWOMBO_API UGameplayAbility_Death : public UGameplayAbility_HitReact
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_Death();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnDeathFinishedExternalEvent(FGameplayEventData Payload);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitFinishDeathEvent;

	virtual void PlayMontage_Implementation(UAnimMontage* Montage) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAbilityTask_PlayMontageAndWait> HitReactMontageTask;

private:
	UFUNCTION()
	void OnHitReactMontageCompleted();

	UFUNCTION()
	void OnHitReactMontageInterrupted();

	UFUNCTION()
	void OnHitReactMontageCancelled();
};
