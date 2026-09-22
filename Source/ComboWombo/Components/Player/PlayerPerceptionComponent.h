#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "PlayerPerceptionComponent.generated.h"

USTRUCT()
struct FPlayerPerceptionCache
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> Actors;

	float UpdateTime = 0.f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBOWOMBO_API UPlayerPerceptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerPerceptionComponent();

	const TArray<AActor*>& GetObservedActors();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float ObservedActorsCacheLifetime = 0.25f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float MaxSightDistance = 3500.f;
	
	// Used to discover objects around the player, including behind them.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<ECollisionChannel>> ObjectChannels;
	
	// used to do an actual visibility check
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UFUNCTION(BlueprintNativeEvent)
	bool IsRelevant(AActor* Actor) const;
	virtual bool IsRelevant_Implementation(AActor* Actor) const;
	
private:
	UPROPERTY()
	FPlayerPerceptionCache AllActorsPerceptionCache;
	
};
