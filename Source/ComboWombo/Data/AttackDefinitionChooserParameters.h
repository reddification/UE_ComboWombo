#pragma once

#include "AttackDefinitionChooserParameters.generated.h"

UENUM()
enum class EComboAttackIndex
{
	Attack1 = 1,
	Attack2 = Attack1 << 1,
	Attack3 = Attack2 << 1,
	Attack4 = Attack3 << 1,
	Attack5 = Attack4 << 1,
	Attack6 = Attack5 << 1,
	Attack7 = Attack6 << 1,
	Attack8 = Attack7 << 1,
	Attack9 = Attack8 << 1,
	Attack10 = Attack9 << 1,
	Attack11 = Attack10 << 1,
	Attack12 = Attack11 << 1,
}; 

ENUM_CLASS_FLAGS(EComboAttackIndex);

USTRUCT(BlueprintType)
struct FAttackDefinitionChooserParameters
{
	GENERATED_BODY()
	
	// Distance from requestor to attack target
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Distance = 150.f;
	
	// Angle between requestor forward vector and attack target
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Angle = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ComboCount = 0;
	
	// Continuation restrictions. If not empty - only attack with one of those tags is allowed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer Continuations;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EComboAttackIndex ComboAttackIndex;
};
