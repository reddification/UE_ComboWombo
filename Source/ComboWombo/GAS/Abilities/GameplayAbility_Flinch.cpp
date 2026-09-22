#include "GameplayAbility_Flinch.h"
#include "Animation/AnimInstance.h"
#include "Data/CWGameplayTags.h"

UGameplayAbility_Flinch::UGameplayAbility_Flinch()
{
	SetAssetTags(CWGameplayTags::Ability_Flinch.GetTag().GetSingleTagContainer());
}

void UGameplayAbility_Flinch::PlayMontage_Implementation(UAnimMontage* Montage)
{
	UAnimInstance* AnimInstance = CurrentActorInfo ? CurrentActorInfo->GetAnimInstance() : nullptr;
	// Keep the ASC's tracked montage owned by stagger/other sustained abilities.
	// Flinch montages must use a separate slot group and contain no root motion.
	const bool bPlayedMontage = AnimInstance && AnimInstance->Montage_Play(Montage) > 0.f;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, !bPlayedMontage);
}
