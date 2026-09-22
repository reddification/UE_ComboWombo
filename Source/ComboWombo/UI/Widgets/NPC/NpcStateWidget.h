// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NpcStateWidget.generated.h"

/**
 * 
 */
UCLASS()
class COMBOWOMBO_API UNpcStateWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetNPC(AActor* InNPC);
};
