#include "Subsystems/HitStopSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"

void UHitStopSubsystem::TryHitStop(float Chance, float Duration)
{
	UWorld* World = GetWorld();
	if (!World || World->GetNetMode() != NM_Standalone || !FMath::IsFinite(Chance)
		|| !FMath::IsFinite(Duration) || Chance <= 0.f || Duration <= 0.f)
		return;

	if (Chance < 1.f && FMath::FRand() >= Chance)
		return;

	AWorldSettings* Settings = World->GetWorldSettings();
	if (!Settings || !Settings->bAllowTimeDilation)
		return;

	if (!bHitStopActive)
	{
		PreviousTimeDilation = Settings->TimeDilation;
		AppliedTimeDilation = Settings->SetTimeDilation(FMath::Min(PreviousTimeDilation, 0.0001f));
		EndRealTime = World->GetRealTimeSeconds() + Duration;
		bHitStopActive = true;
	}
	else
	{
		// Overlapping hits extend the stop without adding their full durations together.
		EndRealTime = FMath::Max(EndRealTime, World->GetRealTimeSeconds() + Duration);
	}
}

void UHitStopSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// World delta and ordinary world timers are slowed by our own time dilation.
	if (GetWorld()->GetRealTimeSeconds() >= EndRealTime)
		RestoreTimeDilation();
}

void UHitStopSubsystem::RestoreTimeDilation()
{
	if (!bHitStopActive)
		return;

	if (AWorldSettings* Settings = GetWorld()->GetWorldSettings())
	{
		// Do not overwrite a new time dilation chosen by another system during the stop.
		if (Settings->TimeDilation == AppliedTimeDilation)
			Settings->SetTimeDilation(PreviousTimeDilation);
	}
	bHitStopActive = false;
}

void UHitStopSubsystem::Deinitialize()
{
	RestoreTimeDilation();
	Super::Deinitialize();
}

bool UHitStopSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UHitStopSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UHitStopSubsystem, STATGROUP_Tickables);
}
