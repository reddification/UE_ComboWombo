#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HitStopSubsystem.generated.h"

/** Owns whole-world hit stop for the single-player combat prototype. */
UCLASS()
class COMBOWOMBO_API UHitStopSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// Chance 0 never triggers; chance 1 always triggers. Duration is in real seconds.
	UFUNCTION(BlueprintCallable)
	void TryHitStop(float Chance, float Duration);

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bHitStopActive; }
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual TStatId GetStatId() const override;
	virtual void Deinitialize() override;

protected:
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

private:
	void RestoreTimeDilation();

	bool bHitStopActive = false;
	double EndRealTime = 0.0;
	float PreviousTimeDilation = 1.f;
	float AppliedTimeDilation = 1.f;
};
