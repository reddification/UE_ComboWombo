// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ComboWindow.generated.h"

/**
 * Overlapping windows will override one another
 * So if anim tracks look like this:
 * -----CW1============CW1------------------
 * ------------CW2=============CW2----------
 * CW1 combo window starts. If input happens now - CW1 combo path is chosen
 * CW2 combo window starts. CW1 combo window is effectively closed. If input happens now - CW2 combo path is chosen
 * CW1 ends, nothing happens no matter if/when input was made
 * CW2 ends, nothing happens no matter if/when input was made
 * So this anim notify only tells the UGameplayAbility_Attack combo window start, duration and potential continuations
 * It is an AN state and not just AN for convenience of setting combo window time frame instead of providing float Duration parameter
 * However the ability itself can change combo window duration to its liking
 * 
 */
UCLASS()
class COMBOWOMBO_API UAnimNotifyState_ComboWindow : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	// If empty - any attack would do. Otherwise - any of those listed in container
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer Continuations;	
};
