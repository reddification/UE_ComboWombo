#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"

template<typename TGameplayAbilityTargetDataStruct>
const TGameplayAbilityTargetDataStruct* GetGameplayEventData(const FGameplayAbilityTargetDataHandle& TargetData)
{
	if (!TargetData.IsValid(0))
		return nullptr; 
		
	const auto Data = TargetData.Get(0);
	if (Data)
	{
		if (ensure(Data->GetScriptStruct() == TGameplayAbilityTargetDataStruct::StaticStruct()))
		{
			const TGameplayAbilityTargetDataStruct* ActivationData = static_cast<const TGameplayAbilityTargetDataStruct*>(Data);
			return ActivationData;
		}
	}

	return nullptr;
}
