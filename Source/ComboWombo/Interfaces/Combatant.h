// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CombatDataTypes.h"
#include "UObject/Interface.h"
#include "Combatant.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCombatant : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMBOWOMBO_API ICombatant
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnKilledActor(AActor* KilledActor, const FReceivedHitData& LastHit);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnStaggeredActor(AActor* StaggeredActor, const FReceivedHitData& LastHit);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetBestTarget() const;	
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsAlly(AActor* OtherActor) const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAttackStarted();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAttackEnded();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsAttacking() const;
	
};
