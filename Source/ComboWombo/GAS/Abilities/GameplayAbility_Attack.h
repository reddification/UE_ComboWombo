#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Data/AttackDefinitionChooserParameters.h"
#include "Data/CombatDataTypes.h"
#include "GameplayAbility_Attack.generated.h"

class UMotionWarpingComponent;
class ACWCharacterBase;
class UAttackDefinition;
class UAbilityTask_MeleeSweep;
class UCombatStyle;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitInputPress;

UCLASS()
class COMBOWOMBO_API UGameplayAbility_Attack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_Attack();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config")
	TObjectPtr<UCombatStyle> CombatStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
	TEnumAsByte<ECollisionChannel> MeleeSweepTraceChannel;
	
	// Multiplies combo window length by this value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(UIMin = 0., ClampMin = 0.))
	double ComboWindowScale = 1.;
	
	// Infinite effect while attack is active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Effects")
	TSubclassOf<UGameplayEffect> AttackActiveEffect;
	
	// Instant effect applied to actor receiving hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Effects")
	TSubclassOf<UGameplayEffect> DamageEffect;
	
	// Used for ignoring attacks if they overlaped arms 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Damage")
	TArray<FName> SkeletonArmsRoots;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Damage")
	bool bAllowTeamDamage = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Montage")
	float MontagePlayRate = 1.f;
	
	// Name of warp target for root motion adjustment. Warp anim notify state must be set on montages
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Montage|Warp")
	FName WarpTargetName = FName("Attack");

	// Follow the target's root transform during warping, retaining the initial relative attack offset.
	// Target movement after setup can exceed MaxExtraWarpDistance.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Montage|Warp")
	bool bAlignWithMovingTarget = false;
	
	// Maximum added travel beyond authored root displacement at warp setup.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config|Montage|Warp", meta=(ClampMin="0"))
	float MaxExtraWarpDistance = 50.f;
	
	// Fallback distance for attack selection and the warp target when there's no active target.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Montage")
	float FallbackAttackDistance = 150.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config|Debug")
	bool Debug_DrawCollision = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Owner")
	TObjectPtr<ACWCharacterBase> OwnerCharacter = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Owner")
	TObjectPtr<USkeletalMeshComponent> OwnerSkeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Owner")
	TObjectPtr<UMotionWarpingComponent> OwnerMotionWarping; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Owner")
	bool bHasCombatantInterface = false;

	// If a combo continuation was commited during an active combo window
	UPROPERTY(BlueprintReadOnly, Category="State|Combo")
	bool bComboContinuationCommited = false;
	
	// Combo window is active until this game time
	UPROPERTY(BlueprintReadOnly, Category="State|Combo")
	double ComboWindowActiveUntilGameTime = 0.;
	
	// How many attacks already done in current combo
	UPROPERTY(BlueprintReadOnly, Category="State|Combo")
	int CurrentComboCount = 0;
	
	// Combo continuations granted by active combo window, fed to chooser
	UPROPERTY(BlueprintReadOnly, Category="State|Combo")
	FGameplayTagContainer ComboContinuations;
	
	UPROPERTY(BlueprintReadOnly, Category = "State|Attack")
	UAttackDefinition* ActiveAttack;

	UPROPERTY(BlueprintReadWrite, Category="State|Attack")
	AActor* ActiveTarget;
	
	UPROPERTY(BlueprintReadWrite, Category = "State|Attack")
	TSet<AActor*> ActorsHitInThisAttack;
	
	UPROPERTY(BlueprintReadOnly, Category = "Tasks")
	UAbilityTask_PlayMontageAndWait* AttackMontageTask;
	
	UPROPERTY(BlueprintReadOnly, Category = "Tasks")
	UAbilityTask_WaitGameplayEvent* WaitNextAttackRequestTask;
	
	UPROPERTY(BlueprintReadOnly, Category = "Tasks")
	UAbilityTask_WaitGameplayEvent* WaitComboWindowTask;
	
	UPROPERTY(BlueprintReadOnly, Category = "Tasks")
	UAbilityTask_WaitGameplayEvent* WaitComboAdvanceTask;
	
	UPROPERTY(BlueprintReadOnly, Category = "Tasks")
	UAbilityTask_WaitGameplayEvent* WaitMeleeSweepRequestTask;
	
	UPROPERTY(BlueprintReadOnly, Category = "Tasks")
	UAbilityTask_MeleeSweep* MeleeSweepTask;

	UFUNCTION(BlueprintNativeEvent)
	void OnNextAttackRequested(FGameplayEventData Payload);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnComboWindowOpened(FGameplayEventData Payload);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnComboAdvanceRequested(FGameplayEventData Payload);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnMontageCompleted();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnMontageInterrupted();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnMontageCancelled();

	UFUNCTION(BlueprintNativeEvent)
	void OnObstacleHit(const FHitResult& Hit);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnActorHit(const FHitResult& Hit);
	
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldIgnoreHit(const FHitResult& Hit);

	// Target-relative reaction: Front/Back are the strike's origin; Left/Right are flinch movement.
	UFUNCTION(BlueprintNativeEvent)
	FGameplayTag GetImpactDirection(const FHitResult& HitResult);
	
	UFUNCTION(BlueprintNativeEvent)
	FReceivedHitData GetDealtHitData(const FHitResult& HitResult);
	
	UFUNCTION(BlueprintNativeEvent)
	AActor* FindBestTarget();

	UFUNCTION(BlueprintNativeEvent)
	void PreAttackMontageStart();

	UFUNCTION(BlueprintNativeEvent)
	void AdvanceCombo();

	UFUNCTION(BlueprintNativeEvent)
	float GetDamageEffectLevel() const;

	UFUNCTION(BlueprintNativeEvent)
	void StartMeleeSweep();
	
	UFUNCTION(BlueprintNativeEvent)
	void StopMeleeSweep();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnAttackWhiffed();
	
	// World-space direction used for attack selection and warping without an active target.
	// Return zero to keep the current facing; player abilities override this with movement intent.
	UFUNCTION(BlueprintNativeEvent)
	FVector GetAttackDirectionWithoutTarget() const;
	
	UFUNCTION(BlueprintNativeEvent)
	bool CanContinueCombo();
	
	UFUNCTION(BlueprintNativeEvent)
	void ApplyHitToTarget(UAbilitySystemComponent* TargetASC, const FHitResult& Hit);
	
	UFUNCTION(BlueprintNativeEvent)
	FAttackDefinitionChooserParameters GetAttackChooserParameters();
	
	UPROPERTY(BlueprintReadOnly)
	FActiveGameplayEffectHandle AttackActiveEffectHandle;

	UFUNCTION(BlueprintNativeEvent)
	void OnMeleeSweepRequested(FGameplayEventData Payload);
	
	UFUNCTION(BlueprintNativeEvent)
	void CommitComboContinuation();
	
	UFUNCTION(BlueprintNativeEvent)
	void UpdateAttackWarpTarget();
	
};
