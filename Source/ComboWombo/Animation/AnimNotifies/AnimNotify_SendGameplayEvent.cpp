#include "AnimNotify_SendGameplayEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (EventTag.IsValid())
	{
		if (auto ASCInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
		{
			if (auto ASC = ASCInterface->GetAbilitySystemComponent())
			{
				FGameplayEventData GameplayEventData;
				ASC->HandleGameplayEvent(EventTag, &GameplayEventData);
			}
		}
	}
}

FString UAnimNotify_SendGameplayEvent::GetNotifyName_Implementation() const
{
	return DisplayName;
}
