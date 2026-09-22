// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_AttackPhase.h"
#include "Components/MeleeCombatComponent.h"
#include "Data/CombatDataTypes.h"

void UAnimNotify_AttackPhase::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	auto MeshOwner = MeshComp->GetOwner();
	if (!MeshOwner)
		return;
	
	auto MeleeCombatComponent = MeshOwner->FindComponentByClass<UMeleeCombatComponent>();
	if (!MeleeCombatComponent)
		return;
	
	switch (AttackPhase)
	{
		case EAttackPhase::None: ensure(false); break;
		case EAttackPhase::WindUp:
			MeleeCombatComponent->BeginWindUp();
			break;
		case EAttackPhase::Release:
			MeleeCombatComponent->BeginRelease();
			break;
		case EAttackPhase::Recover:
			MeleeCombatComponent->BeginRecover();
			break;
		default: ensure(false); break;
	}
}

void UAnimNotify_AttackPhase::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	auto MeshOwner = MeshComp->GetOwner();
	if (!MeshOwner)
		return;
	
	auto MeleeCombatComponent = MeshOwner->FindComponentByClass<UMeleeCombatComponent>();
	if (!MeleeCombatComponent)
		return;
	
	switch (AttackPhase)
	{
		case EAttackPhase::None: ensure(false); break;
		case EAttackPhase::WindUp:
			MeleeCombatComponent->EndWindUp();
			break;
		case EAttackPhase::Release:
			MeleeCombatComponent->EndRelease();
			break;
		case EAttackPhase::Recover:
			MeleeCombatComponent->EndRecover();
			break;
		default: ensure(false); break;
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}