// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility_Attack.h"
#include "GameplayAbility_Attack_Player.generated.h"

/**
 * 
 */
UCLASS()
class COMBOWOMBO_API UGameplayAbility_Attack_Player : public UGameplayAbility_Attack
{
	GENERATED_BODY()
	
protected:
	virtual FVector GetAttackDirectionWithoutTarget_Implementation() const override;
};
