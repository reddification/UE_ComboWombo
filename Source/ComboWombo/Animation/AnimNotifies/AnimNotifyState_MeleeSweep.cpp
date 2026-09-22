// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_MeleeSweep.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Data/CWGameplayTags.h"
#include "GAS/Abilities/GameplayAbility_Death.h"
#include "GAS/Data/GameplayAbilityTargetData_Bool.h"

void UAnimNotifyState_MeleeSweep::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                              const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	SendGameplayEvent(MeshComp, true);
}

void UAnimNotifyState_MeleeSweep::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	SendGameplayEvent(MeshComp, false);
}

void UAnimNotifyState_MeleeSweep::SendGameplayEvent(USkeletalMeshComponent* MeshComp, bool bOpen) const
{
	if (auto OwnerASCInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
	{
		if (auto ASC = OwnerASCInterface->GetAbilitySystemComponent())
		{
			FGameplayEventData GameplayEventData;
			GameplayEventData.Instigator = MeshComp->GetOwner();
			FGameplayAbilityTargetData_Bool* BoolData = new FGameplayAbilityTargetData_Bool(bOpen);
			GameplayEventData.TargetData.Add(BoolData);
			ASC->HandleGameplayEvent(CWGameplayTags::Ability_Attack_Event_MeleeSweep, &GameplayEventData);
		}
	}
}