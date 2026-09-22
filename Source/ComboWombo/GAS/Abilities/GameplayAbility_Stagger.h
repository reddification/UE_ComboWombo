#pragma once

#include "GameplayAbility_HitReact.h"
#include "GameplayAbility_Stagger.generated.h"

UCLASS()
class COMBOWOMBO_API UGameplayAbility_Stagger : public UGameplayAbility_HitReact
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_Stagger();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
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
