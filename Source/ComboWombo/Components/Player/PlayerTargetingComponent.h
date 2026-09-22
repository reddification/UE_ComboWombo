#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerTargetingComponent.generated.h"

/** Selects a melee target from perceived actors using planar input alignment and distance. */
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class COMBOWOMBO_API UPlayerTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerTargetingComponent();

	AActor* FindBestTarget(const TArray<AActor*>& Candidates, const FVector& DesiredDirection) const;

protected:
	/** Maximum actor-center distance in cm. Also normalizes the distance score. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Targeting", meta=(ClampMin="1", Units="cm"))
	float MaxTargetDistance = 400.f;

	/** Maximum horizontal angle away from desired direction. Outside targets are always rejected. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Targeting", meta=(ClampMin="0", ClampMax="89", Units="deg"))
	float TargetConeHalfAngle = 60.f;

	/** 0 favors nearest; 1 favors best aligned. Cone and range filters always apply. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Targeting", meta=(ClampMin="0", ClampMax="1"))
	float AlignmentWeight = 0.5f;
};
