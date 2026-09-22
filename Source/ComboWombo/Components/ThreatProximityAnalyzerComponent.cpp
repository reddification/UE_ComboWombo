#include "Components/ThreatProximityAnalyzerComponent.h"

#include "Characters/CWCharacterBase.h"
#include "GameFramework/Character.h"
#include "Interfaces/CharacterPerception.h"
#include "Interfaces/CombatAnimInstance.h"

DEFINE_LOG_CATEGORY(LogCombat_ThreatProximity)

UThreatProximityAnalyzerComponent::UThreatProximityAnalyzerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 0.15f;
}

void UThreatProximityAnalyzerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()->Implements<UCharacterPerception>())
	{
		SetComponentTickEnabled(false);
		return;	
	}
	
	OwnerCharacter = Cast<ACWCharacterBase>(GetOwner());
	if (OwnerCharacter == nullptr)
	{
		SetComponentTickEnabled(false);
		return;		
	}
	
	auto AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr || !AnimInstance->Implements<UCombatAnimInstance>())
	{
		SetComponentTickEnabled(false);
		return;		
	}

	CombatAnimInstance.SetObject(AnimInstance);
	CombatAnimInstance.SetInterface(Cast<ICombatAnimInstance>(AnimInstance));
	
	ConsideredCloseCombatRangeSq = ConsideredCloseCombatRange * ConsideredCloseCombatRange;
	OwnerCharacter->DeathStartedEvent.AddDynamic(this, &UThreatProximityAnalyzerComponent::OnOwnerDied);
}

void UThreatProximityAnalyzerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(OwnerCharacter))
		OwnerCharacter->DeathStartedEvent.RemoveAll(this);	
	
	Super::EndPlay(EndPlayReason);
}

void UThreatProximityAnalyzerComponent::OnOwnerDied_Implementation(AActor* Victim, AActor* Killer, FGameplayTag DeathCause)
{
	SetComponentTickEnabled(false);
}

void UThreatProximityAnalyzerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TRACE_CPUPROFILER_EVENT_SCOPE(UThreatProximityAnalyzerComponent::TickComponent)
	
	const TArray<AActor*>& EnemiesInSight = ICharacterPerception::Execute_GetEnemies(OwnerCharacter);
		
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	// OwnerViewRotation.Vector();
	const FVector OwnerViewDirection = GetOwner()->GetActorForwardVector();
	const float DotProductThreshold = FMath::Cos(FMath::DegreesToRadians(ThreatAngleDegrees));
	bool bEnemyClose = false;
	for (const auto* Enemy : EnemiesInSight)
	{
		UE_VLOG_LOCATION(GetOwner(), LogCombat_ThreatProximity, Verbose, Enemy->GetActorLocation(), 25, FColor::Yellow, TEXT("Enemy %s"), *Enemy->GetName());
		
		const FVector EnemyLocation = Enemy->GetActorLocation();
		const float DotProduct = OwnerViewDirection | (EnemyLocation - OwnerLocation).GetSafeNormal();
		if (DotProduct >= DotProductThreshold)
		{
			UE_VLOG(GetOwner(), LogCombat_ThreatProximity, VeryVerbose, TEXT("dot product to %s passes. dp = %.2f"), *Enemy->GetName(), DotProduct);
			
			const float EnemyToOwnerDistSq = (EnemyLocation - OwnerLocation).SizeSquared();
			if (EnemyToOwnerDistSq < ConsideredCloseCombatRangeSq)
			{
				UE_VLOG(GetOwner(), LogCombat_ThreatProximity, VeryVerbose, TEXT("%s closer than threshold range. distance = %.2f"), *Enemy->GetName(), FMath::Sqrt(EnemyToOwnerDistSq));
				bEnemyClose = true;
				break;			
			}
		}
	}

	if (bEnemyClose)
	{
		UE_VLOG(GetOwner(), LogCombat_ThreatProximity, VeryVerbose, TEXT("Enemy is close. Ready up weapon"));
        ICombatAnimInstance::Execute_SetCombatStanceActive(CombatAnimInstance.GetObject(), true);
		RemainingDelaySwitchToThreatIsFar = SwitchToThreatIsFarDelay;
	}
	else
	{
		RemainingDelaySwitchToThreatIsFar -= DeltaTime;
		if (RemainingDelaySwitchToThreatIsFar <= 0.f)
		{
			UE_VLOG(GetOwner(), LogCombat_ThreatProximity, VeryVerbose, TEXT("No enemy nearby. Unready weapon"));
			ICombatAnimInstance::Execute_SetCombatStanceActive(CombatAnimInstance.GetObject(), false);
		}
	}
}
