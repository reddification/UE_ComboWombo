#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CombatSettings.generated.h"

/** Project-wide combat tuning, exposed in Project Settings > Game > Combat. */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Combat"))
class COMBOWOMBO_API UCombatSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// Applies when the player staggers another actor. Zero never triggers; one always triggers.
	UPROPERTY(Config, EditAnywhere, Category = "Hit Stop|Stagger", meta = (ClampMin = "0", ClampMax = "1"))
	float StaggerHitStopChance = 0.35f;

	// Real seconds; zero disables stagger hit stop.
	UPROPERTY(Config, EditAnywhere, Category = "Hit Stop|Stagger", meta = (ClampMin = "0", Units = "s"))
	float StaggerHitStopDuration = 0.06f;

	// Applies when the player kills another actor. Zero never triggers; one always triggers.
	UPROPERTY(Config, EditAnywhere, Category = "Hit Stop|Death", meta = (ClampMin = "0", ClampMax = "1"))
	float DeathHitStopChance = 1.f;

	// Real seconds; zero disables death hit stop.
	UPROPERTY(Config, EditAnywhere, Category = "Hit Stop|Death", meta = (ClampMin = "0", Units = "s"))
	float DeathHitStopDuration = 0.1f;
};
