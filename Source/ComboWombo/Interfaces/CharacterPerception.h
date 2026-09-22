// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterPerception.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCharacterPerception : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMBOWOMBO_API ICharacterPerception
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<AActor*> GetEnemies() const;
};
