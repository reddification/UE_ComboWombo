#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "NpcInfoWidgetComponent.generated.h"


class UNpcStateWidget;
struct FNpcDeathEventData;
class UNpcAttitudesComponent;
class UNpcComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBOWOMBO_API UNpcInfoWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UNpcInfoWidgetComponent();
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ConsiderableDistance = 3000.f;

	// Maximum angle from the player's view direction to the NPC, in degrees (not the full cone angle).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.0", ClampMax="180.0", Units="Degrees"))
	float MaxAngleToShowWidget = 120.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float VisibilityUpdateInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UFUNCTION(BlueprintNativeEvent)
	void OnDeathStarted(AActor* Victim, AActor* Killer, FGameplayTag DeathCause);

	UFUNCTION(BlueprintNativeEvent)
	bool IsMustBeVisible() const;
	
private:
	FTimerHandle UpdateVisibilityTimer;
	void UpdateVisibility();
	
	void InitializeNpc();
	
	TWeakObjectPtr<class APawn> PlayerPawn;
	TWeakObjectPtr<UNpcStateWidget> NpcStateWidget;
};
