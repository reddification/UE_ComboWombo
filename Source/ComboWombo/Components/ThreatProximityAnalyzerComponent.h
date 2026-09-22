#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Characters/CWCharacterBase.h"
#include "Components/ActorComponent.h"
#include "ThreatProximityAnalyzerComponent.generated.h"

class ICharacterPerception;
class ICombatAnimInstance;

COMBOWOMBO_API DECLARE_LOG_CATEGORY_EXTERN(LogCombat_ThreatProximity, Verbose, All);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBOWOMBO_API UThreatProximityAnalyzerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UThreatProximityAnalyzerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float ConsideredCloseCombatRange = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float SwitchToThreatIsFarDelay = 3.f;

	/** Maximum angle from the owner's view direction to a threat, in degrees. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f, UIMax = 180.f, ClampMax = 180.f, Units = "deg"))
	float ThreatAngleDegrees = 75.f;

	UFUNCTION(BlueprintNativeEvent)
	void OnOwnerDied(AActor* Victim, AActor* Killer, FGameplayTag DeathCause);
	
private:
	float ConsideredCloseCombatRangeSq = 500.f * 500.f;
	float RemainingDelaySwitchToThreatIsFar = 0.f;
	
	UPROPERTY()
	TScriptInterface<ICombatAnimInstance> CombatAnimInstance = nullptr;
	
	UPROPERTY()
	ACWCharacterBase* OwnerCharacter;
};
