// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "Data/CombatDataTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/CharacterPerception.h"
#include "Interfaces/Combatant.h"
#include "Interfaces/Damageable.h"
#include "CWCharacterBase.generated.h"

class UMotionWarpingComponent;
class UGameplayEffect;
class UMovementAttributeSet;
class UVitalsAttributeSet;
class UCWAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDeathStartedEvent, AActor*, Victim, AActor*, Killer, FGameplayTag, DeathCause);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDeathEndedEvent, AActor*, Victim, AActor*, Killer, FGameplayTag, DeathCause);

UCLASS()
class COMBOWOMBO_API ACWCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICharacterPerception,
	public IGameplayTagAssetInterface, public IDamageable, public ICombatant
{
	GENERATED_BODY()

public:
	ACWCharacterBase();
	virtual void Tick(float DeltaSeconds) override;
	virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime = 0.f) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FDeathStartedEvent DeathStartedEvent;
	
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FDeathEndedEvent DeathEndedEvent;

	UFUNCTION(BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable)
	void PlaySpeakerAudio(USoundBase* Sound);
	
	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UCWAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UVitalsAttributeSet> VitalsAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UMovementAttributeSet> MovementAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAudioComponent> AudioSpeakerComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State")
	FGameplayTagContainer StateTags;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State")
	FReceivedHitData LastHitData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State")
	bool bDeathFinished = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Death")
	bool bEnableRagdollOnDeath = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Death")
	bool bClearAnimInstanceOnDeath = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Body")
	FName HeadRegionStartBone = FName("neck_01");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Body")
	FName TorsoRegionStartBone = FName("spine_01");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn = true), Category="Personality")
	FText Name;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> InAirStateEffect;

	UFUNCTION(BlueprintNativeEvent)
	void UpdateRotationControl();
	
	UFUNCTION(BlueprintNativeEvent)
	void SetAttackRotationControl(bool bEnabled);
	
	bool bCameraControlledStrafing = false;
	
private:
	bool bAttackRotationControl = false;
	bool bSavedControllerYaw = false;
	bool bSavedOrientToMovement = false;
	bool bSavedControllerDesiredRotation = false;

	void ActivateRagdoll(bool bClearAnimInstanceOnDeath);
	void DisableRagdollAfterDeath();
	
	FTimerHandle DisableRagdollTimer;
	FActiveGameplayEffectHandle InAirActiveEffectHandle;
	
public: // IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
public: // IDamageable
	virtual FReceivedHitData GetLastHitData_Implementation() override;
	virtual void OnReceivedHit_Implementation(const FReceivedHitData& ReceivedHitData) override;
	virtual void OnDeathStarted_Implementation() override;
	virtual void OnDeathFinished_Implementation() override;
	
public: // IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
public: // ICombatant
	virtual bool IsAttacking_Implementation() const override;
};
