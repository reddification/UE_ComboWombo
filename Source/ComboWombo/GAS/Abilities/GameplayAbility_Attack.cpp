#include "GameplayAbility_Attack.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Chooser.h"
#include "MotionWarpingComponent.h"
#include "Characters/CWCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Data/AttackDefinition.h"
#include "Data/CombatStyle.h"
#include "Data/CWGameplayTags.h"
#include "Data/LogChannels.h"
#include "GameFramework/Character.h"
#include "GAS/Data/GameplayAbilityTargetData_Bool.h"
#include "GAS/Data/GameplayAbilityTargetData_ComboWindow.h"
#include "GAS/Tasks/AbilityTask_MeleeSweep.h"
#include "Helpers/GASHelpers.h"
#include "Interfaces/Combatant.h"
#include "Interfaces/Damageable.h"

UGameplayAbility_Attack::UGameplayAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGameplayAbility_Attack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) && IsValid(CombatStyle);
}

void UGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentComboCount = 0;
	ComboContinuations = CWGameplayTags::Combat_Attack_ComboStarter.GetTag().GetSingleTagContainer();
	AdvanceCombo();
	
	if (!IsActive() || bIsAbilityEnding)
		return;
	
	WaitNextAttackRequestTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CWGameplayTags::Ability_Attack_Event_Combo_Request);
	WaitNextAttackRequestTask->EventReceived.AddDynamic(this, &UGameplayAbility_Attack::OnNextAttackRequested);
	WaitNextAttackRequestTask->ReadyForActivation();
	
	WaitComboAdvanceTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CWGameplayTags::Ability_Attack_Event_Combo_Advance);
	WaitComboAdvanceTask->EventReceived.AddDynamic(this, &UGameplayAbility_Attack::OnComboAdvanceRequested);
	WaitComboAdvanceTask->ReadyForActivation();
	
	WaitComboWindowTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CWGameplayTags::Ability_Attack_Event_Combo_Window);
	WaitComboWindowTask->EventReceived.AddDynamic(this, &UGameplayAbility_Attack::OnComboWindowOpened);
	WaitComboWindowTask->ReadyForActivation();
	
	WaitMeleeSweepRequestTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CWGameplayTags::Ability_Attack_Event_MeleeSweep);
	WaitMeleeSweepRequestTask->EventReceived.AddDynamic(this, &UGameplayAbility_Attack::OnMeleeSweepRequested);
	WaitMeleeSweepRequestTask->ReadyForActivation();
	
	if (bHasCombatantInterface)
		ICombatant::Execute_OnAttackStarted(ActorInfo->AvatarActor.Get());
	
}

void UGameplayAbility_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!IsEndAbilityValid(Handle, ActorInfo))
		return;
	
	if (IsValid(OwnerMotionWarping))
		OwnerMotionWarping->RemoveWarpTarget(WarpTargetName);
	
	if (bHasCombatantInterface)
		ICombatant::Execute_OnAttackEnded(ActorInfo->AvatarActor.Get());
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (AttackActiveEffectHandle.IsValid())
	{
		if (ActorInfo->AbilitySystemComponent.IsValid())
			ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(AttackActiveEffectHandle);
	}
}

bool UGameplayAbility_Attack::CanContinueCombo_Implementation()
{
	// for convenience of working with EComboAttackIndex, indexing starts with 0, but to check against max combo count we add +1
	return bComboContinuationCommited && CurrentComboCount + 1 < CombatStyle->MaxComboCount;
}

void UGameplayAbility_Attack::OnComboWindowOpened_Implementation(FGameplayEventData Payload)
{
	if (bComboContinuationCommited)
	{
		UE_LOG(LogCW_Attack, Log, TEXT("Combo window opened but combo continuation already commited. Not proceeding"))
		return;
	}
	
	if (auto ComboWindowData = GetGameplayEventData<FGameplayAbilityTargetData_ComboWindow>(Payload.TargetData))
	{
		ComboWindowActiveUntilGameTime = GetWorld()->GetTimeSeconds() + ComboWindowData->Duration * ComboWindowScale;
		ComboContinuations = ComboWindowData->Continuations;
		UE_LOG(LogCW_Attack, Log, TEXT("Combo window opened. Continuations: %s; Active for %.2f s"), *ComboContinuations.ToStringSimple(), ComboWindowData->Duration * ComboWindowScale)
	}
}

FVector UGameplayAbility_Attack::GetAttackDirectionWithoutTarget_Implementation() const
{
	return GetAvatarActorFromActorInfo()->GetActorForwardVector();
}

float UGameplayAbility_Attack::GetDamageEffectLevel_Implementation() const
{
	return GetAbilityLevel();
}

void UGameplayAbility_Attack::PreAttackMontageStart_Implementation()
{
	ActorsHitInThisAttack.Reset();
	UpdateAttackWarpTarget();
}

void UGameplayAbility_Attack::CommitComboContinuation_Implementation()
{
	bComboContinuationCommited = true;
	UE_LOG(LogCW_Attack, Log, TEXT("Combo commited. Continations: %s"), *ComboContinuations.ToStringSimple())
}

void UGameplayAbility_Attack::UpdateAttackWarpTarget_Implementation()
{
	if (!OwnerMotionWarping)
		return;
	
	OwnerMotionWarping->RemoveWarpTarget(WarpTargetName);
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!IsValid(Avatar) || !IsValid(ActiveAttack))
		return;

	if (!IsValid(ActiveTarget))
	{
		FVector Direction = GetAttackDirectionWithoutTarget().GetSafeNormal2D();
		if (Direction.IsNearlyZero())
			Direction = Avatar->GetActorForwardVector().GetSafeNormal2D();

		// Snapshot movement intent per strike, using the same direction as the attack chooser.
		OwnerMotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName,
			Avatar->GetActorLocation() + Direction * FMath::Max(0.f, FallbackAttackDistance),
			Direction.Rotation());
		return;
	}
	
	if (IsValid(ActiveTarget))
	{
		float RootTravel = 0.f;
		if (!ActiveAttack->GetRootTravelToWarpTarget(WarpTargetName, RootTravel))
		{
			UE_LOG(LogCW_Attack, Verbose, TEXT("%s has no cached single %s warp window; continuing the attack without warping."),
				*GetNameSafe(ActiveAttack), *WarpTargetName.ToString());
			return;
		}
		
		const FVector Origin = Avatar->GetActorLocation();
		const FVector ToTarget = ActiveTarget->GetActorLocation() - Origin;
		const FVector Direction = ToTarget.GetSafeNormal2D();
		
		if (Direction.IsNearlyZero())
			return;
		
		float CapsuleSeparation = 0.f;
		if (OwnerCharacter)
			CapsuleSeparation += OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
		
		if (const ACharacter* TargetCharacter = Cast<ACharacter>(ActiveTarget))
			CapsuleSeparation += TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
		
		const float StandOff = FMath::Max(ActiveAttack->ContactDistance, CapsuleSeparation + 5.f);
		// Clamp initial travel; following a moving target can move the endpoint farther during the strike.
		const float MeshScale = OwnerSkeletalMesh ? OwnerSkeletalMesh->GetComponentScale().GetAbsMax() : 1.f;
		const float MaxTravel = RootTravel * MeshScale + FMath::Max(0.f, MaxExtraWarpDistance);
		const float Travel = FMath::Clamp(static_cast<float>(ToTarget.Size2D()) - StandOff, 0.f, MaxTravel);
		const FTransform WarpTransform(Direction.Rotation(), Origin + Direction * Travel);

		if (bAlignWithMovingTarget)
		{
			if (const USceneComponent* TargetRoot = ActiveTarget->GetRootComponent(); IsValid(TargetRoot))
			{
				// Keep the initial spacing and facing instead of warping into the target's capsule center.
				const FTransform RelativeWarpTransform = WarpTransform.GetRelativeTransform(TargetRoot->GetComponentTransform());
				OwnerMotionWarping->AddOrUpdateWarpTargetFromComponent(WarpTargetName, TargetRoot, NAME_None, true,
					EWarpTargetLocationOffsetDirection::TargetsForwardVector,
					RelativeWarpTransform.GetLocation(), RelativeWarpTransform.Rotator());
				return;
			}
		}
		
		OwnerMotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName, Origin + Direction * Travel, Direction.Rotation());
	}
}

void UGameplayAbility_Attack::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	bHasCombatantInterface = ActorInfo->AvatarActor.IsValid() && ActorInfo->AvatarActor->Implements<UCombatant>();
	if (auto Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		OwnerSkeletalMesh = Character->GetMesh();
	
	OwnerCharacter = Cast<ACWCharacterBase>(ActorInfo->AvatarActor.Get());
	OwnerMotionWarping = ActorInfo->AvatarActor->FindComponentByClass<UMotionWarpingComponent>();
}

FReceivedHitData UGameplayAbility_Attack::GetDealtHitData_Implementation(const FHitResult& HitResult)
{
	FReceivedHitData HitData;
	HitData.BoneName = HitResult.BoneName;
	HitData.Causer = GetAvatarActorFromActorInfo();
	HitData.HitLocation = HitResult.Location;
	HitData.HitNormal = HitResult.Normal;
	HitData.HitType = ActiveAttack->DamageType;
	HitData.ImpactDirectionTag = GetImpactDirection(HitResult);
	
	return HitData;
}

FGameplayTag UGameplayAbility_Attack::GetImpactDirection_Implementation(const FHitResult& HitResult)
{
	const AActor* Target = HitResult.GetActor();
	if (!IsValid(Target))
		return CWGameplayTags::HitDirection_Front;

	// The socket sweep records fist travel, whereas surface normals depend on the geometry hit.
	FVector StrikeDirection = (HitResult.TraceEnd - HitResult.TraceStart).GetSafeNormal2D();
	if (StrikeDirection.IsNearlyZero() && !HitResult.bStartPenetrating)
		StrikeDirection = (-HitResult.ImpactNormal).GetSafeNormal2D();

	if (StrikeDirection.IsNearlyZero())
	{
		const AActor* Attacker = GetAvatarActorFromActorInfo();
		if (IsValid(Attacker))
			StrikeDirection = (Target->GetActorLocation() - Attacker->GetActorLocation()).GetSafeNormal2D();
	}

	if (StrikeDirection.IsNearlyZero())
		return CWGameplayTags::HitDirection_Front;

	// Use target yaw: pitch/roll and vertical fist movement do not select a different horizontal reaction.
	const FRotator TargetYaw(0.0, Target->GetActorRotation().Yaw, 0.0);
	const FVector LocalStrikeDirection = TargetYaw.UnrotateVector(StrikeDirection);

	// Four equal sectors; exact diagonals prefer Front/Back.
	// Front/Back name the strike's origin, Left/Right name the victim's flinch movement.
	// Thus a left hook from in front flinches Left, and the same hook from behind flinches Right.
	if (FMath::Abs(LocalStrikeDirection.X) >= FMath::Abs(LocalStrikeDirection.Y))
		return LocalStrikeDirection.X < 0.0 ? CWGameplayTags::HitDirection_Front : CWGameplayTags::HitDirection_Back;

	return LocalStrikeDirection.Y < 0.0 ? CWGameplayTags::HitDirection_Left : CWGameplayTags::HitDirection_Right;
}

void UGameplayAbility_Attack::OnMeleeSweepRequested_Implementation(FGameplayEventData Payload)
{
	auto BoolParameter = GetGameplayEventData<FGameplayAbilityTargetData_Bool>(Payload.TargetData);
	// if parameter not specified - consider toggle mode
	bool bStartSweep = false;
	if (BoolParameter == nullptr)
		bStartSweep = MeleeSweepTask == nullptr;
	else 
		bStartSweep = BoolParameter->bValue;
	
	if (bStartSweep)
		StartMeleeSweep();
	else
		StopMeleeSweep();
}

void UGameplayAbility_Attack::StartMeleeSweep_Implementation()
{
	if (IsValid(MeleeSweepTask))
		MeleeSweepTask->ExternalCancel();
	
	MeleeSweepTask = UAbilityTask_MeleeSweep::MeleeSweep(this, "MeleeSweep", ActiveAttack->MeleeSweepSocketName, 
		ActiveAttack->MeleeSweepRadius, OwnerSkeletalMesh, MeleeSweepTraceChannel, Debug_DrawCollision);
	if (ensure(MeleeSweepTask))
	{
		MeleeSweepTask->HitDamageableEvent.AddDynamic(this, &UGameplayAbility_Attack::OnActorHit);
		MeleeSweepTask->HitImmoveableEvent.AddDynamic(this, &UGameplayAbility_Attack::OnObstacleHit);
		MeleeSweepTask->ReadyForActivation();
	}
}

void UGameplayAbility_Attack::StopMeleeSweep_Implementation()
{
	if (IsValid(MeleeSweepTask))
	{
		MeleeSweepTask->ExternalCancel();
		MeleeSweepTask = nullptr;
		
		if (IsValid(AttackMontageTask) && AttackMontageTask->IsActive() && ActorsHitInThisAttack.IsEmpty())
			OnAttackWhiffed();
	}
}

void UGameplayAbility_Attack::OnAttackWhiffed_Implementation()
{
}

void UGameplayAbility_Attack::OnComboAdvanceRequested_Implementation(FGameplayEventData Payload)
{
	if (CanContinueCombo())
	{
		CurrentComboCount++;
		AdvanceCombo();
	}	
}

void UGameplayAbility_Attack::AdvanceCombo_Implementation()
{
	FChooserEvaluationContext Context;
	ActiveTarget = FindBestTarget();
	auto Params = GetAttackChooserParameters();
	Context.AddStructParam(Params);
	FEvaluateChooser AttackChooser(CombatStyle->Attacks);
	ActiveAttack = Cast<UAttackDefinition>(AttackChooser.ChooseObject(Context));
	
	bComboContinuationCommited = false;
	ComboWindowActiveUntilGameTime = 0.;
	ComboContinuations.Reset();
	
	bool bHasActiveAttack = AttackMontageTask != nullptr && AttackMontageTask->IsActive();
	if (ActiveAttack == nullptr)
	{
		UE_LOG(LogCW_Attack, Warning, TEXT("No attack found in chooser. Can't advance combo"));
		if (!bHasActiveAttack)
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		
		return;
	}
	
	bool bCommited = CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, nullptr);
	if (!bCommited)
	{
		UE_LOG(LogCW_Attack, Warning, TEXT("Commit ability failed. Can't advance combo"));
		if (!bHasActiveAttack)
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		
		return;
	}

	if (bHasActiveAttack)
	{
		AttackMontageTask->OnCompleted.Clear();
		AttackMontageTask->OnCancelled.Clear();
		AttackMontageTask->OnInterrupted.Clear();
		AttackMontageTask->ExternalCancel();
		
		if (IsValid(MeleeSweepTask))
		{
			MeleeSweepTask->ExternalCancel();
			MeleeSweepTask = nullptr;
		}
	}
	
	if (bHasCombatantInterface)
		ICombatant::Execute_OnAttackStarted(GetAvatarActorFromActorInfo());
	
	PreAttackMontageStart();

	AttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, "Attack", 
		ActiveAttack->Montage, MontagePlayRate, NAME_None, true, 1.f);
	AttackMontageTask->OnCompleted.AddDynamic(this, &UGameplayAbility_Attack::OnMontageCompleted);
	AttackMontageTask->OnInterrupted.AddDynamic(this, &UGameplayAbility_Attack::OnMontageInterrupted);
	AttackMontageTask->OnCancelled.AddDynamic(this, &UGameplayAbility_Attack::OnMontageCancelled);
	AttackMontageTask->ReadyForActivation();
}

FAttackDefinitionChooserParameters UGameplayAbility_Attack::GetAttackChooserParameters_Implementation()
{
	auto AvatarActor = GetAvatarActorFromActorInfo();
	FAttackDefinitionChooserParameters Result;
	if (ActiveTarget == nullptr)
	{
		Result.Distance = FallbackAttackDistance;
		const FVector Direction = GetAttackDirectionWithoutTarget();
		if (!Direction.IsNearlyZero())
		{
			Result.Angle = FMath::RadiansToDegrees(FMath::Acos(AvatarActor->GetActorForwardVector().GetSafeNormal2D() | Direction.GetSafeNormal2D()));
			if ((AvatarActor->GetActorRightVector().GetSafeNormal2D() | Direction.GetSafeNormal2D()) < 0.f)
				Result.Angle *= -1;
		}
	}
	else
	{
		FVector AvatarToTarget = ActiveTarget->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation();
		Result.Distance = AvatarToTarget.Size2D();
		Result.Angle = FMath::RadiansToDegrees(FMath::Acos(AvatarActor->GetActorForwardVector() | AvatarToTarget.GetSafeNormal2D()));
	}
	
	Result.ComboCount = CurrentComboCount;
	Result.Continuations = ComboContinuations;
	Result.ComboAttackIndex = static_cast<EComboAttackIndex>(1 << CurrentComboCount);
	
	return Result;
}

void UGameplayAbility_Attack::OnObstacleHit_Implementation(const FHitResult& Hit)
{
	if (ActiveAttack->bStopSweepOnHitObstacle)
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGameplayAbility_Attack::OnActorHit_Implementation(const FHitResult& Hit)
{
	if (ShouldIgnoreHit(Hit))
		return;
	
	auto HitActor = Hit.GetActor();
	if (HitActor->Implements<UDamageable>())
	{
		FReceivedHitData ReceivedHitData = GetDealtHitData(Hit);
		IDamageable::Execute_OnReceivedHit(HitActor, ReceivedHitData);
		ActorsHitInThisAttack.Add(HitActor);
	}

	if (auto TargetASCInterface = Cast<IAbilitySystemInterface>(HitActor))
		if (auto TargetASC = TargetASCInterface->GetAbilitySystemComponent())
			ApplyHitToTarget(TargetASC, Hit);
	
	if (ActiveAttack->bStopSweepOnHitEnemy)
	{
		MeleeSweepTask->ExternalCancel();
		MeleeSweepTask = nullptr;
	}
}

void UGameplayAbility_Attack::ApplyHitToTarget_Implementation(UAbilitySystemComponent* TargetASC, const FHitResult& Hit)
{
	if (IsValid(DamageEffect))
	{
		auto OwnerAvatarActor = GetAvatarActorFromActorInfo();
		auto EffectContext = CurrentActorInfo->AbilitySystemComponent->MakeEffectContext();
		auto EffectSpec = CurrentActorInfo->AbilitySystemComponent->MakeOutgoingSpec(DamageEffect, GetDamageEffectLevel(), EffectContext);
		EffectSpec.Data->SetByCallerTagMagnitudes = ActiveAttack->Damages;
		EffectContext.AddHitResult(Hit);
		EffectContext.AddInstigator(OwnerAvatarActor, OwnerAvatarActor);
		// TODO consider making setting last hit data and applying effect an atomic operation
		CurrentActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*EffectSpec.Data, TargetASC);
	}
			
	TargetASC->TryActivateAbilitiesByTag(CWGameplayTags::Ability_Flinch.GetTag().GetSingleTagContainer());
}

bool UGameplayAbility_Attack::ShouldIgnoreHit_Implementation(const FHitResult& Hit)
{
	auto HitActor = Hit.GetActor();
	if (HitActor == nullptr || HitActor == GetAvatarActorFromActorInfo() || ActorsHitInThisAttack.Contains(HitActor))
		return true;
	
	bool bAlly = bHasCombatantInterface && ICombatant::Execute_IsAlly(GetAvatarActorFromActorInfo(), HitActor);
	if (bAlly && !bAllowTeamDamage)
		return true;
	
	auto HitCharacter = Cast<ACharacter>(HitActor);
	if (ActiveAttack->bIgnoreArms)
		for (const auto& ArmRootBoneName : SkeletonArmsRoots)
			if (Hit.BoneName == ArmRootBoneName || HitCharacter->GetMesh()->BoneIsChildOf(Hit.BoneName, ArmRootBoneName))
				return true;
	
	return false;
}

void UGameplayAbility_Attack::OnNextAttackRequested_Implementation(FGameplayEventData Payload)
{
	if (GetWorld()->GetTimeSeconds() <= ComboWindowActiveUntilGameTime)
		CommitComboContinuation();
}

AActor* UGameplayAbility_Attack::FindBestTarget_Implementation()
{
	return bHasCombatantInterface ? ICombatant::Execute_GetBestTarget(GetAvatarActorFromActorInfo()) : nullptr;
}

void UGameplayAbility_Attack::OnMontageCompleted_Implementation()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGameplayAbility_Attack::OnMontageInterrupted_Implementation()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGameplayAbility_Attack::OnMontageCancelled_Implementation()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}
