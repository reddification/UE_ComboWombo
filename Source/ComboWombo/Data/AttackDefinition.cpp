// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackDefinition.h"
#include "Animation/AnimMontage.h"
#include "MotionWarpingComponent.h"
#include "AnimNotifyState_MotionWarping.h"
#include "CWGameplayTags.h"

UAttackDefinition::UAttackDefinition()
{
	
}

bool UAttackDefinition::GetRootTravelToWarpTarget(FName WarpTargetName, float& Distance) const
{
	Distance = 0.f;
	if (const float* CachedDistance = RootTravelByWarpTarget.Find(WarpTargetName))
	{
		Distance = *CachedDistance;
		return true;
	}

	return false;
}

void UAttackDefinition::RebuildRootTravelCache()
{
	RootTravelByWarpTarget.Reset();
	if (!IsValid(Montage))
		return;

	// A referenced montage may still be awaiting PostLoad when this data asset loads.
	Montage->ConditionalPostLoad();
	if (!Montage->SlotAnimTracks.IsEmpty())
	{
		for (const auto& Segment : Montage->SlotAnimTracks[0].AnimTrack.AnimSegments)
		{
			if (auto Animation = Segment.GetAnimReference())
				Animation->ConditionalPostLoad();
		}
	}

	TArray<FMotionWarpingWindowData> Windows;
	UMotionWarpingUtilities::GetMotionWarpingWindowsFromAnimation(Montage, Windows);
	TMap<FName, float> WindowEndTimes;
	TSet<FName> DuplicateTargets;
	for (const auto& Window : Windows)
	{
		const auto Modifier = Window.AnimNotify ? Cast<URootMotionModifier_Warp>(Window.AnimNotify->RootMotionModifier) : nullptr;
		if (!Modifier || Modifier->WarpTargetName.IsNone())
			continue;

		const FName TargetName = Modifier->WarpTargetName;
		if (WindowEndTimes.Contains(TargetName))
			DuplicateTargets.Add(TargetName);
		else
			WindowEndTimes.Add(TargetName, Window.EndTime);
	}

	for (const auto& Window : WindowEndTimes)
	{
		if (DuplicateTargets.Contains(Window.Key))
			continue;

		const float Distance = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Montage, 0.f, Window.Value).GetTranslation().Size2D();
		RootTravelByWarpTarget.Add(Window.Key, Distance);
	}
}

void UAttackDefinition::PostLoad()
{
	Super::PostLoad();
	RebuildRootTravelCache();
}

void UAttackDefinition::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);
	RebuildRootTravelCache();
}

#if WITH_EDITOR
void UAttackDefinition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	RebuildRootTravelCache();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UAttackDefinition::PostEditUndo()
{
	Super::PostEditUndo();
	RebuildRootTravelCache();
}
#endif

FPrimaryAssetId UAttackDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("CW.Attack", SystemName);
}
