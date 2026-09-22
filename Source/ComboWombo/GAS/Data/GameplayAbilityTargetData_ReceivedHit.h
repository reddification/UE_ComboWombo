// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayAbilityTargetData_ReceivedHit.generated.h"

USTRUCT(BlueprintType)
struct COMBOWOMBO_API FGameplayAbilityTargetData_ReceivedHit : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:
	FGameplayAbilityTargetData_ReceivedHit() {};

	UPROPERTY(BlueprintReadOnly)
	FVector HitLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector HitNormal = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag HitDirectionTag;
	
	UPROPERTY(BlueprintReadOnly)
	float HealthDamage = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* Causer = nullptr;
	
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_ReceivedHit::StaticStruct();
	}

	// bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	// {
	// 	// The engine already defined NetSerialize for FName & FPredictionKey, thanks Epic!
	// 	HitLocation.NetSerialize(Ar, Map, bOutSuccess);
	// 	HitNormal.NetSerialize(Ar, Map, bOutSuccess);
	// 	HitDirectionTag.NetSerialize(Ar, Map, bOutSuccess);
	// 	Ar << HealthDamage;
	// 	
	// 	bOutSuccess = true;
	// 	return true;
	// }
};

// template<>
// struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ReceivedHit> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ReceivedHit>
// {
// 	enum
// 	{
// 		WithNetSerializer = true // This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
// 	};
// };