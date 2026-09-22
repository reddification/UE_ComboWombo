#include "AnimNotifyState_ComboWindow.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Data/CWGameplayTags.h"
#include "GAS/Data/GameplayAbilityTargetData_ComboWindow.h"

void UAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                               const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (auto OwnerASCInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
	{
		if (auto ASC = OwnerASCInterface->GetAbilitySystemComponent())
		{
			FGameplayEventData GameplayEventData;
			GameplayEventData.Instigator = MeshComp->GetOwner();
			auto Parameters = new FGameplayAbilityTargetData_ComboWindow();
			Parameters->Duration = TotalDuration;
			Parameters->Continuations = Continuations;
			GameplayEventData.TargetData.Add(Parameters);
			ASC->HandleGameplayEvent(CWGameplayTags::Ability_Attack_Event_Combo_Window, &GameplayEventData);
		}
	}
}
