#include "PlayerPerceptionComponent.h"

#include "Characters/CWCharacterBase.h"
#include "Engine/OverlapResult.h"

class UMeleeCombatSettings;

UPlayerPerceptionComponent::UPlayerPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UPlayerPerceptionComponent::IsRelevant_Implementation(AActor* Actor) const
{
	auto GameCharacter = Cast<ACWCharacterBase>(Actor);
	if (GameCharacter == nullptr || GameCharacter->IsDead())
		return false;
		
	return true;
}

const TArray<AActor*>& UPlayerPerceptionComponent::GetObservedActors()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UPlayerPerceptionComponent::GetObservedActors)

	float WorldTimeNow = GetWorld()->GetTimeSeconds();
	if (AllActorsPerceptionCache.UpdateTime + ObservedActorsCacheLifetime > WorldTimeNow)
		return AllActorsPerceptionCache.Actors;
	
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(UPlayerPerceptionComponent::GetObservedActors::UpdateCache)
	
		AllActorsPerceptionCache.Actors.Reset();
		
		TArray<FOverlapResult> Overlaps;
		FVector OwnerEyesLocation;
		FRotator OwnerViewRotation;
		GetOwner()->GetActorEyesViewPoint(OwnerEyesLocation, OwnerViewRotation);
		// Directional attacks can target any side of the player. Selection applies its own cone.
		FVector Location = GetOwner()->GetActorLocation();
		FCollisionShape OverlapShape = FCollisionShape::MakeBox(FVector(MaxSightDistance, MaxSightDistance, 400.f));
		FQuat Rotation = FQuat::Identity;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		FCollisionObjectQueryParams CollisionObjectQueryParams;
		for (const auto& ObjectChannel : ObjectChannels)
			CollisionObjectQueryParams.AddObjectTypesToQuery(ObjectChannel.GetValue());

		// DrawDebugBox(GetWorld(), Location, FVector(MaxDistance * 0.5f, MaxDistance * 0.5f, 300.f), Rotation, FColor::Red, false, 1.f, 0, 5);
	
		bool bOverlapped = GetWorld()->OverlapMultiByObjectType(Overlaps, Location, Rotation, CollisionObjectQueryParams, OverlapShape, CollisionQueryParams);
		if (!bOverlapped)
		{
			AllActorsPerceptionCache.UpdateTime = WorldTimeNow;
			return AllActorsPerceptionCache.Actors;
		}
		
		AllActorsPerceptionCache.Actors.Reserve(6);
		TSet<const AActor*> ProcessedActors;
		ProcessedActors.Reserve(FMath::CeilToInt32(FMath::Sqrt(static_cast<float>(Overlaps.Num()))));
		ProcessedActors.Add(GetOwner());
		//overlaps may have multiple components for the same actor
		for (const auto& Overlap : Overlaps)
		{
			auto OverlapActor = Overlap.GetActor();
			if (ProcessedActors.Contains(OverlapActor))
				continue;

			ProcessedActors.Add(OverlapActor);
			if (OverlapActor == nullptr || !OverlapActor->HasActorBegunPlay() || !IsRelevant(OverlapActor))
				continue;
			
			FHitResult HitResult;
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, OwnerEyesLocation, OverlapActor->GetActorLocation(),
															 TraceChannel, CollisionQueryParams);
			if (bHit && HitResult.GetActor() == OverlapActor)
				AllActorsPerceptionCache.Actors.Add(OverlapActor);
		}
	
		AllActorsPerceptionCache.UpdateTime = WorldTimeNow;
		return AllActorsPerceptionCache.Actors;
	}
}
