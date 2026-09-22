#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Data/AbilitySet.h"
#include "CWAbilitySystemComponent.generated.h"

class UCharacterMovementComponent;
class UAbilitySet;
struct FOnAttributeChangeData;

USTRUCT(BlueprintType)
struct FOwnedAbilityDescriptor
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle AbilitySpec;
};

UCLASS()
class COMBOWOMBO_API UCWAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAbilitySet> AbilitySet;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FAbilitySet_GrantedHandles GrantedAbilitySet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> RestorePoiseEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RestorePoiseEffectStartDelay = 3.f;
	
	UFUNCTION(BlueprintNativeEvent)
	void StartDeath();
	
	UFUNCTION(BlueprintNativeEvent)
	void OutOfPoise();

private:
	void BindAttributeDelegates();
	void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
	void OnMovementSpeedChanged(const FOnAttributeChangeData& ChangeData);
	void OnTurnRateChanged(const FOnAttributeChangeData& ChangeData);
	void OnPoiseChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	
	void OnRestorableAttributeChanged(float NewValue, float OldValue, float MaxValue,
		FTimerHandle& RestoreStartTimer, float RestoreEffectActivationDelay,
		const TSubclassOf<UGameplayEffect>& RestoreEffect, FActiveGameplayEffectHandle& ActiveRestoreAttributeEffectSpec);
	
	void StartRestoreAttribute(const TSubclassOf<UGameplayEffect>* RestoreGameplayEffect, FActiveGameplayEffectHandle* ActiveRestoreEffectSpec);
	
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MovementSpeedChangedDelegateHandle;
	FDelegateHandle TurnRateChangedDelegateHandle;
	
	FTimerHandle StartRestorePoiseTimer;
	FActiveGameplayEffectHandle ActiveRestorePoiseEffectSpec;
	
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CMC;
};
