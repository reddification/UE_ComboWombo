// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayAbilityTargetData_ComboWindow.generated.h"

USTRUCT(BlueprintType)
struct COMBOWOMBO_API FGameplayAbilityTargetData_ComboWindow : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:
	FGameplayAbilityTargetData_ComboWindow() {};

	UPROPERTY(BlueprintReadOnly)
	float Duration = 0.5f;
	
	UPROPERTY(BlueprintReadOnly)
    FGameplayTagContainer Continuations;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_ComboWindow::StaticStruct();
	}
};
