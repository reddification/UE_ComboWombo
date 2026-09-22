// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayAbilityTargetData_Bool.generated.h"

USTRUCT(BlueprintType)
struct COMBOWOMBO_API FGameplayAbilityTargetData_Bool : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:
	FGameplayAbilityTargetData_Bool() {};
	FGameplayAbilityTargetData_Bool(bool bValue_In) : bValue(bValue_In) {};

	UPROPERTY(BlueprintReadOnly)
	bool bValue = false;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Bool::StaticStruct();
	}
};