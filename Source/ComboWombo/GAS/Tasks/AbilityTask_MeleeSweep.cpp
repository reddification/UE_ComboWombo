#include "AbilityTask_MeleeSweep.h"

#include "DrawDebugHelpers.h"
#include "Interfaces/Damageable.h"

UAbilityTask_MeleeSweep::UAbilityTask_MeleeSweep()
{
	bTickingTask = true;
}

UAbilityTask_MeleeSweep* UAbilityTask_MeleeSweep::MeleeSweep(UGameplayAbility* OwningAbility, FName TaskInstanceName, 
	FName SocketName, float SphereRadius, const USkeletalMeshComponent* Mesh, TEnumAsByte<ECollisionChannel> TraceChannel, bool bDrawDebug)
{
	if (Mesh == nullptr || !Mesh->DoesSocketExist(SocketName))
		return nullptr;
	
	UAbilityTask_MeleeSweep* NewTask = NewAbilityTask<UAbilityTask_MeleeSweep>(OwningAbility, TaskInstanceName);
	NewTask->Mesh = Mesh;
	NewTask->SphereRadius = SphereRadius;
	NewTask->SocketName = SocketName;
	NewTask->PreviousSocketLocation = Mesh->GetSocketLocation(SocketName);
	NewTask->Shape = FCollisionShape::MakeSphere(SphereRadius);
	NewTask->TraceChannel = TraceChannel;
	NewTask->CollisionQueryParams.AddIgnoredActor(Mesh->GetOwner());
	NewTask->bDrawDebug = bDrawDebug;
	return NewTask;
}

void UAbilityTask_MeleeSweep::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (!Mesh.IsValid())
		return;
	
	TArray<FHitResult> HitResults;
	FVector CurrentLocation = Mesh->GetSocketLocation(SocketName);
	// The return value only reports blocking hits; overlaps still populate HitResults.
	GetWorld()->SweepMultiByChannel(HitResults, PreviousSocketLocation, CurrentLocation, FQuat::Identity,
		TraceChannel, Shape, CollisionQueryParams);
	
	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!IsValid(HitActor))
			continue;

		if (HitActor->Implements<UDamageable>())
			HitDamageableEvent.Broadcast(HitResult);
		else if (HitResult.bBlockingHit)
			HitImmoveableEvent.Broadcast(HitResult);
	}
	
	if (bDrawDebug)
	{
		constexpr float DebugLifetime = 2.f;
		const FVector SweepDelta = CurrentLocation - PreviousSocketLocation;
		const FVector SweepCenter = (PreviousSocketLocation + CurrentLocation) * 0.5f;
		const FQuat SweepRotation = SweepDelta.IsNearlyZero()
			? FQuat::Identity
			: FQuat::FindBetweenNormals(FVector::UpVector, SweepDelta.GetSafeNormal());
		// Capsule half-height includes the spherical end caps.
		DrawDebugCapsule(GetWorld(), SweepCenter, SweepDelta.Size() * 0.5f + SphereRadius,
			SphereRadius, SweepRotation, FColor::Green, false, DebugLifetime);

		for (const FHitResult& HitResult : HitResults)
		{
			const FColor HitColor = HitResult.bBlockingHit ? FColor::Red : FColor::Yellow;
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.f, 12,
				HitColor, false, DebugLifetime);
		}
	}
	
	PreviousSocketLocation = CurrentLocation;
}

void UAbilityTask_MeleeSweep::ExternalCancel()
{
	Super::ExternalCancel();
	HitDamageableEvent.Clear();
	HitImmoveableEvent.Clear();
}
