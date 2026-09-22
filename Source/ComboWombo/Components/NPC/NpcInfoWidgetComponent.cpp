// Fill out your copyright notice in the Description page of Project Settings.

#include "NpcInfoWidgetComponent.h"

#include "Characters/CWCharacterBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/NPC/NpcStateWidget.h"

UNpcInfoWidgetComponent::UNpcInfoWidgetComponent()
{
	Space = EWidgetSpace::Screen;	
}

void UNpcInfoWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UNpcInfoWidgetComponent::InitializeNpc);
}

void UNpcInfoWidgetComponent::InitializeNpc()
{
	NpcStateWidget = Cast<UNpcStateWidget>(GetWidget());
	if (!NpcStateWidget.IsValid())
		return;
	
	if (auto OwnerCharacter = Cast<ACWCharacterBase>(GetOwner()))
	{
		if (OwnerCharacter->IsDead())
		{
			SetVisibility(false);
			return;
		}
		
		OwnerCharacter->DeathStartedEvent.AddDynamic(this, &UNpcInfoWidgetComponent::OnDeathStarted);
	}
	
	NpcStateWidget->SetNPC(GetOwner());
	PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	GetWorld()->GetTimerManager().SetTimer(UpdateVisibilityTimer, this, &UNpcInfoWidgetComponent::UpdateVisibility,
		VisibilityUpdateInterval, true);

	SetVisibility(false);
}

void UNpcInfoWidgetComponent::OnDeathStarted_Implementation(AActor* Victim, AActor* Killer, FGameplayTag DeathCause)
{
	SetVisibility(false);
	GetWorld()->GetTimerManager().ClearTimer(UpdateVisibilityTimer);
}

bool UNpcInfoWidgetComponent::IsMustBeVisible_Implementation() const
{
	bool bMustBeVisible = false;
	float DistanceToPlayerSq = (GetOwner()->GetActorLocation() - PlayerPawn->GetActorLocation()).SizeSquared();
	if (DistanceToPlayerSq < ConsiderableDistance * ConsiderableDistance)
	{
		FVector ViewLocation;
		FRotator ViewRotation;
		PlayerPawn->GetActorEyesViewPoint(ViewLocation, ViewRotation);
		const float MinViewDotProduct = FMath::Cos(FMath::DegreesToRadians(MaxAngleToShowWidget));
		if ((ViewRotation.Vector() | (GetOwner()->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal()) > MinViewDotProduct)
		{
			FHitResult HitResult;
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.AddIgnoredActor(GetOwner());
			const FVector StartLocation = GetOwner()->GetActorLocation();
			const FVector EndLocation = PlayerPawn->GetActorLocation();
				
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation + (EndLocation - StartLocation).GetSafeNormal() * 50.f, 
				TraceChannel, CollisionQueryParams);

			bMustBeVisible = !bHit;// && HitResult.GetActor() == PlayerPawn;
		}
	}
	
	return bMustBeVisible;
}

void UNpcInfoWidgetComponent::UpdateVisibility()
{
	SetVisibility(PlayerPawn.IsValid() && IsMustBeVisible());
}
