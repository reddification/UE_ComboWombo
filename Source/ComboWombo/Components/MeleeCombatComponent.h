// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeCombatComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBOWOMBO_API UMeleeCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMeleeCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void BeginWindUp();
	
	UFUNCTION(BlueprintNativeEvent)
	void BeginRelease();

	UFUNCTION(BlueprintNativeEvent)
	void BeginRecover();
	
	UFUNCTION(BlueprintNativeEvent)
	void EndWindUp();
	
	UFUNCTION(BlueprintNativeEvent)
	void EndRelease();
	
	UFUNCTION(BlueprintNativeEvent)
	void EndRecover();
};
