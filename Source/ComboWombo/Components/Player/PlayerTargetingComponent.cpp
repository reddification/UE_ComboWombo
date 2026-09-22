#include "PlayerTargetingComponent.h"

#include "Characters/CWCharacterBase.h"

UPlayerTargetingComponent::UPlayerTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

AActor* UPlayerTargetingComponent::FindBestTarget(const TArray<AActor*>& Candidates, const FVector& DesiredDirection) const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner) || MaxTargetDistance <= 0.f)
		return nullptr;

	FVector Direction = DesiredDirection.GetSafeNormal2D();
	if (Direction.IsNearlyZero())
		Direction = Owner->GetActorForwardVector().GetSafeNormal2D();
	if (Direction.IsNearlyZero())
		return nullptr;

	const double MinDot = FMath::Cos(FMath::DegreesToRadians(FMath::Clamp(TargetConeHalfAngle, 0.f, 89.f)));
	const double Weight = FMath::Clamp(AlignmentWeight, 0.f, 1.f);
	AActor* BestTarget = nullptr;
	double BestScore = -1.0;
	double BestDistance = TNumericLimits<double>::Max();

	for (AActor* Candidate : Candidates)
	{
		if (!IsValid(Candidate) || Candidate == Owner)
			continue;
		// Perception is cached; an observed character may have died since its last update.
		const ACWCharacterBase* Character = Cast<ACWCharacterBase>(Candidate);
		if (Character && Character->IsDead())
			continue;

		const FVector Offset = Candidate->GetActorLocation() - Owner->GetActorLocation();
		const double Distance = Offset.Size();
		const FVector TargetDirection = Offset.GetSafeNormal2D();
		if (Distance > MaxTargetDistance || TargetDirection.IsNearlyZero())
			continue;

		const double Dot = FMath::Clamp(FVector::DotProduct(Direction, TargetDirection), -1.0, 1.0);
		if (Dot < MinDot)
			continue;

		const double DistanceScore = 1.0 - Distance / MaxTargetDistance;
		const double Score = Weight * Dot + (1.0 - Weight) * DistanceScore;
		if (Score > BestScore || (Score == BestScore && Distance < BestDistance))
		{
			BestTarget = Candidate;
			BestScore = Score;
			BestDistance = Distance;
		}
	}

	return BestTarget;
}
