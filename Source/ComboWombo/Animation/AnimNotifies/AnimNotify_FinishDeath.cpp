// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FinishDeath.h"

#include "Characters/CWCharacterBase.h"

void UAnimNotify_FinishDeath::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp->GetOwner()->Implements<UDamageable>())
		IDamageable::Execute_OnDeathFinished(MeshComp->GetOwner());
}
