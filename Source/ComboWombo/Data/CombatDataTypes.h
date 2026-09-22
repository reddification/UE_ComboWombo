#pragma once
#include "GameplayTagContainer.h"
#include "CombatDataTypes.generated.h"

UENUM(BlueprintType)
enum class EAttackPhase : uint8
{
	None,
	WindUp,
	Release,
	Recover
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None,
	Melee,
	Ranged,
};

UENUM(BlueprintType)
enum class EWeaponHitSituation : uint8
{
	None,
	WeaponClash,
	AttackBlocked,
	AttackParried,
	Body,
	ImmovableObject
};

UENUM(BlueprintType)
enum class EAttackStepDirection : uint8
{
	None,
	Forward,
	Left,
	Right,
	Back
};

USTRUCT(BlueprintType)
struct FReceivedHitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ImpactDirectionTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag HitType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HitNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HitLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Causer;

	// derived
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag BodyRegion;
	
	// derived
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double GameTime;
};