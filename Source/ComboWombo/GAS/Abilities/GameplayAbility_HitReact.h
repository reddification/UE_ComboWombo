#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_HitReact.generated.h"

class UChooserTable;
class UAnimMontage;

UCLASS()
class COMBOWOMBO_API UGameplayAbility_HitReact : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_HitReact();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// Subclasses own playback and decide when the reaction ability ends.
	UFUNCTION(BLueprintNativeEvent, BlueprintCallable)
	void PlayMontage(UAnimMontage* Montage);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit React")
	TObjectPtr<UChooserTable> MontageChooser;

	// Infinite, non-stacking effect whose lifetime belongs to this ability.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit React")
	TSubclassOf<UGameplayEffect> ActiveEffect;

private:
	FActiveGameplayEffectHandle ActiveEffectHandle;
};
