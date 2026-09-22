// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CombatStyle.generated.h"

class UChooserTable;
/**
 * 
 */
UCLASS()
class COMBOWOMBO_API UCombatStyle : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SystemName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChooserTable* Attacks;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WindupRate = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReleaseRate = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RecoverRate = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MaxComboCount = 3;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
