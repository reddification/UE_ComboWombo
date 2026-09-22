// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/CombatDataTypes.h"
#include "Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMBOWOMBO_API IDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnReceivedHit(const FReceivedHitData& ReceivedHitData);
	 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FReceivedHitData GetLastHitData();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnStaggerStarted();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnStaggerFinished();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnDeathStarted();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnDeathFinished();
};
