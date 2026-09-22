#include "CWCharacterBase.h"

#include "MotionWarpingComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/CWGameplayTags.h"
#include "Data/LogChannels.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/CWAbilitySystemComponent.h"
#include "GAS/Attributes/MovementAttributeSet.h"
#include "GAS/Attributes/VitalsAttributeSet.h"

// Sets default values
ACWCharacterBase::ACWCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<UCWAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	VitalsAttributeSet = CreateDefaultSubobject<UVitalsAttributeSet>(TEXT("VitalsAttributeSet"));
	MovementAttributeSet = CreateDefaultSubobject<UMovementAttributeSet>(TEXT("MovementAttributeSet"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	AudioSpeakerComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioSpeakerComponent"));
	AudioSpeakerComponent->SetupAttachment(GetMesh(), "head");
	GetCharacterMovement()->GravityScale = 2.f;
}

void ACWCharacterBase::Tick(float DeltaSeconds)
{
	if (bCameraControlledStrafing || bAttackRotationControl)
		UpdateRotationControl();
	
	Super::Tick(DeltaSeconds);
}

void ACWCharacterBase::FaceRotation(FRotator NewControlRotation, float DeltaTime)
{
	// PlayerController can call this before the character's tick.
	if (bCameraControlledStrafing || bAttackRotationControl)
		UpdateRotationControl();
	
	Super::FaceRotation(NewControlRotation, DeltaTime);
}

void ACWCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto World = GetWorld())
	{
		auto& TimerManager = World->GetTimerManager();
		TimerManager.ClearAllTimersForObject(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

bool ACWCharacterBase::IsDead() const
{
	return VitalsAttributeSet->GetHealth() <= 0.f;;
}

void ACWCharacterBase::PlaySpeakerAudio(USoundBase* Sound)
{
	if (Sound)
	{
		if (AudioSpeakerComponent->IsPlaying())
			AudioSpeakerComponent->Stop();
		
		AudioSpeakerComponent->SetSound(Sound);
		AudioSpeakerComponent->Play();
	}
}

void ACWCharacterBase::SetAttackRotationControl_Implementation(bool bEnabled)
{
	if (bAttackRotationControl == bEnabled)
		return;
	
	if (bEnabled)
	{
		bSavedControllerYaw = bUseControllerRotationYaw;
		bSavedOrientToMovement = GetCharacterMovement()->bOrientRotationToMovement;
		bSavedControllerDesiredRotation = GetCharacterMovement()->bUseControllerDesiredRotation;
	}
	
	bAttackRotationControl = bEnabled;
	if (!bEnabled && !bCameraControlledStrafing)
	{
		bUseControllerRotationYaw = bSavedControllerYaw;
		GetCharacterMovement()->bOrientRotationToMovement = bSavedOrientToMovement;
		GetCharacterMovement()->bUseControllerDesiredRotation = bSavedControllerDesiredRotation;
		return;
	}
	
	UpdateRotationControl();
}

void ACWCharacterBase::UpdateRotationControl_Implementation()
{
	auto CMC = GetCharacterMovement();
	if (bAttackRotationControl)
	{
		bUseControllerRotationYaw = false;
		CMC->bOrientRotationToMovement = false;
		CMC->bUseControllerDesiredRotation = false;
	}
	else if (bCameraControlledStrafing)
	{
		const bool bMoving = GetVelocity().SizeSquared2D() > 1.f ||
			!GetPendingMovementInputVector().IsNearlyZero() || !CMC->GetCurrentAcceleration().IsNearlyZero();
		bUseControllerRotationYaw = bMoving;
		CMC->bOrientRotationToMovement = false;
		CMC->bUseControllerDesiredRotation = bMoving;
	}
	else
	{
		bUseControllerRotationYaw = false;
		CMC->bOrientRotationToMovement = true;
		CMC->bUseControllerDesiredRotation = false;
	}
}


void ACWCharacterBase::Falling()
{
	Super::Falling();
	
	if (!IsValid(InAirStateEffect))
		return;
	
	if (InAirStateEffect->GetDefaultObject<UGameplayEffect>()->DurationPolicy != EGameplayEffectDurationType::Infinite)
	{
		UE_LOG(LogCW, Warning, TEXT("Falling state effect is NOT infinite duration policy. Not applying"))
		return;
	}
	
	auto EffectContext = AbilitySystemComponent->MakeEffectContext();
	auto Spec = AbilitySystemComponent->MakeOutgoingSpec(InAirStateEffect, 1.f, EffectContext);
	InAirActiveEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data);
}

void ACWCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	if (InAirActiveEffectHandle.IsValid())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(InAirActiveEffectHandle);
		InAirActiveEffectHandle.Invalidate();
	}
	
	auto JumpAbilityTags = CWGameplayTags::Ability_Jump.GetTag().GetSingleTagContainer();
	AbilitySystemComponent->CancelAbilities(&JumpAbilityTags);
}

UAbilitySystemComponent* ACWCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FReceivedHitData ACWCharacterBase::GetLastHitData_Implementation()
{
	return LastHitData;
}

bool ACWCharacterBase::IsAttacking_Implementation() const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(CWGameplayTags::State_Attacking);
}

void ACWCharacterBase::OnReceivedHit_Implementation(const FReceivedHitData& ReceivedHitData)
{
	LastHitData = ReceivedHitData;
	if (ReceivedHitData.BoneName == HeadRegionStartBone || GetMesh()->BoneIsChildOf(ReceivedHitData.BoneName, HeadRegionStartBone))
		LastHitData.BodyRegion = CWGameplayTags::BodyRegion_Head;
	else if (ReceivedHitData.BoneName == TorsoRegionStartBone || GetMesh()->BoneIsChildOf(ReceivedHitData.BoneName, TorsoRegionStartBone)) 
		LastHitData.BodyRegion = CWGameplayTags::BodyRegion_Body;
	else
		LastHitData.BodyRegion = CWGameplayTags::BodyRegion_Legs;
	
	LastHitData.GameTime = GetWorld()->GetTimeSeconds();
}

void ACWCharacterBase::OnDeathStarted_Implementation()
{
	DeathStartedEvent.Broadcast(this, LastHitData.Causer, LastHitData.HitType);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);
	
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
}

void ACWCharacterBase::ActivateRagdoll(bool bClearAnimInstance)
{
	if (bClearAnimInstance)
		GetMesh()->SetAnimInstanceClass(nullptr);
	
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetWorld()->GetTimerManager().SetTimer(DisableRagdollTimer, this, &ACWCharacterBase::DisableRagdollAfterDeath, 5.f, false);
}

void ACWCharacterBase::DisableRagdollAfterDeath()
{
	GetMesh()->PutAllRigidBodiesToSleep();
	GetMesh()->SetComponentTickEnabled(false);
}

void ACWCharacterBase::OnDeathFinished_Implementation()
{
	if (bDeathFinished)
		return;
	
	bDeathFinished = true;
	if (bEnableRagdollOnDeath)
		ActivateRagdoll(bClearAnimInstanceOnDeath);
	
	StateTags.AddTag(CWGameplayTags::State_Dead);
}

void ACWCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(StateTags);
	TagContainer.AppendTags(AbilitySystemComponent->GetOwnedGameplayTags());
}
