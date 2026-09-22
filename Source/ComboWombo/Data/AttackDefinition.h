// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttackDefinition.generated.h"

/**
 *  Potentially: damages on hit, on block. 
 */
UCLASS()
class COMBOWOMBO_API UAttackDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UAttackDefinition();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SystemName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	// Desired capsule-center separation at contact, NOT the animation's step distance (cm).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Motion Warping", meta=(ClampMin="0", Units="cm"))
	float ContactDistance = 100.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, float> Damages;	

	// melee, bullet, bludgeon, puncture, cut, etc
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag DamageType;

	// jab, cross, knee.left, elbow.right, etc. Potentially used for chooser to build complex combo-ing logic
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttackType;
	
	// Any tags that can be useful for whatever logic later on
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer AttackTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName MeleeSweepSocketName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MeleeSweepRadius = 15.f;
	
	// Stop attack animation on hitting 1st enemy
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bStopSweepOnHitEnemy = false;
	
	// Stop attack animation on hitting immovable object (e.g. wall)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bStopSweepOnHitObstacle = true;
	
	// Ignore applying damage to arms
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIgnoreArms = true;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	// Returns cached net horizontal root displacement from montage start to the end of its single named warp window.
	// Returns false for missing/multiple windows; put the notify on the montage itself.
	UFUNCTION(BlueprintPure, Category="Motion Warping")
	bool GetRootTravelToWarpTarget(FName WarpTargetName, float& Distance) const;

	// Refresh after editing the referenced montage/animation, or assigning Montage from C++ at runtime.
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Motion Warping")
	void RebuildRootTravelCache();

	virtual void PostLoad() override;
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
#endif
	
private:
	// Derived on load, so existing assets and cooked builds do not require a resave.
	UPROPERTY(VisibleAnywhere, Transient, Category="Motion Warping")
	TMap<FName, float> RootTravelByWarpTarget;
};
