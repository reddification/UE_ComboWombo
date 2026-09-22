// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/CombatAnimInstance.h"
#include "CWAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class COMBOWOMBO_API UCWAnimInstance : public UAnimInstance, public ICombatAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void SetCombatStanceActive_Implementation(bool bReadyUp) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCombatStanceActive = false;
};
