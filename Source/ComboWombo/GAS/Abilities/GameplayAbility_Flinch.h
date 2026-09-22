#pragma once

#include "GameplayAbility_HitReact.h"
#include "GameplayAbility_Flinch.generated.h"

UCLASS()
class COMBOWOMBO_API UGameplayAbility_Flinch : public UGameplayAbility_HitReact
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_Flinch();
	
protected:
	virtual void PlayMontage_Implementation(UAnimMontage* Montage) override;
};
