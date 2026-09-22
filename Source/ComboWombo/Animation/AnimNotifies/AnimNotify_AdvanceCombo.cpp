#include "AnimNotify_AdvanceCombo.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Data/CWGameplayTags.h"
#include "GAS/Abilities/GameplayAbility_Death.h"

void UAnimNotify_AdvanceCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (auto ASCInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
	{
		if (auto ASC = ASCInterface->GetAbilitySystemComponent())
		{
			FGameplayEventData GameplayEventData;
			ASC->HandleGameplayEvent(CWGameplayTags::Ability_Attack_Event_Combo_Advance, &GameplayEventData);
		}
	}
}
