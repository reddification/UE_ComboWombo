// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeCombatComponent.h"


// Sets default values for this component's properties
UMeleeCombatComponent::UMeleeCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMeleeCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMeleeCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMeleeCombatComponent::BeginWindUp_Implementation()
{
}

void UMeleeCombatComponent::BeginRelease_Implementation()
{
}

void UMeleeCombatComponent::BeginRecover_Implementation()
{
}

void UMeleeCombatComponent::EndWindUp_Implementation()
{
}

void UMeleeCombatComponent::EndRelease_Implementation()
{
}

void UMeleeCombatComponent::EndRecover_Implementation()
{
}

