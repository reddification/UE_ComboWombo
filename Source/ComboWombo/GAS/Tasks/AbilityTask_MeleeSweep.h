// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_MeleeSweep.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMeleeSweepDamageable, const FHitResult&, HitResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMeleeSweepImmoveable, const FHitResult&, HitResult);

UCLASS()
class COMBOWOMBO_API UAbilityTask_MeleeSweep : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UAbilityTask_MeleeSweep();
	
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_MeleeSweep* MeleeSweep(UGameplayAbility* OwningAbility, FName TaskInstanceName,
		FName SocketName, float SphereRadius, const USkeletalMeshComponent* Mesh, 
		TEnumAsByte<ECollisionChannel> TraceChannel, bool bDrawDebug = false);
	
	UPROPERTY(BlueprintAssignable)
	FMeleeSweepDamageable HitDamageableEvent;
	
	UPROPERTY(BlueprintAssignable)
	FMeleeSweepImmoveable HitImmoveableEvent;

	virtual void TickTask(float DeltaTime) override;
	
	virtual void ExternalCancel() override;
	
private:
	FName SocketName;
	float SphereRadius = 10.f;
	TArray<FHitResult> SweptActors;
	FVector PreviousSocketLocation = FVector::ZeroVector;
	TWeakObjectPtr<const USkeletalMeshComponent> Mesh;
	FCollisionShape Shape;
	FCollisionQueryParams CollisionQueryParams;
	TEnumAsByte<ECollisionChannel> TraceChannel;
	bool bDrawDebug = false;
};
