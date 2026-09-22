#pragma once

#include "GameplayTagContainer.h"
#include "HitReactChooserParameters.generated.h"

USTRUCT(BlueprintType)
struct COMBOWOMBO_API FHitReactChooserParameters
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer HitType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BodyRegion;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer ImpactDirection;
};
