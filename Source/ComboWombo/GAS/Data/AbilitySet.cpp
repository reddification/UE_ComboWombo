// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySet.h"
#include "AbilitySystemComponent.h"

void FAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle, const FGameplayTag AssociatedTag, int SkillLevel)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(AssociatedTag, FSkillData { SkillLevel, Handle });
	}
}

void FAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set->GetClass());
}

void FAbilitySet_GrantedHandles::TakeFromAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const auto& Handle : AbilitySpecHandles)
	{
		if (Handle.Value.AbilitySpecHandle.IsValid())
		{
			ASC->ClearAbility(Handle.Value.AbilitySpecHandle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (auto SetClass : GrantedAttributeSets)
	{
		auto Set = SetClass.LoadSynchronous()->GetDefaultObject<UAttributeSet>(); 
		ASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UAbilitySet::UAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* ASC, FAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(ASC);
	
	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!ensure(IsValid(AbilityToGrant.Ability)))
			continue;

		bool bAbilityAlreadyGranted = false;
		for (const auto& ExistingAbility : OutGrantedHandles->AbilitySpecHandles)
		{
			auto GrantedAbilitySpec = ASC->FindAbilitySpecFromHandle(ExistingAbility.Value.AbilitySpecHandle);
			if (GrantedAbilitySpec != nullptr && GrantedAbilitySpec->Ability->GetClass() == AbilityToGrant.Ability)
			{
				bAbilityAlreadyGranted = true;
				GrantedAbilitySpec->Level = AbilityToGrant.AbilityLevel;
				break;
			}
		}
		
		if (bAbilityAlreadyGranted)
			continue;
		
		UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();
		
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		FGameplayTag AssociatedTag = AbilityToGrant.AssociatedTag.IsValid() ? AbilityToGrant.AssociatedTag : AbilityCDO->GetAssetTags().First();
		ensure(AssociatedTag.IsValid());
		
		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle, AssociatedTag, AbilityToGrant.AbilityLevel);
			OutGrantedHandles->AbilitySpecsToAssociatedTags.Add(AbilitySpecHandle, AssociatedTag);
		}
	}
	
	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (SetToGrant.AttributeSet.IsNull())
			continue;

		if (GrantedAttributes.Contains(SetToGrant))
			continue;
		
		UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet.LoadSynchronous());
		ASC->AddAttributeSetSubobject(NewSet);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (EffectToGrant.GameplayEffect.IsNull())
			continue;

		auto LoadedEffectClass = EffectToGrant.GameplayEffect.LoadSynchronous();
		const UGameplayEffect* GameplayEffect = LoadedEffectClass->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
}

void UAbilitySet::SetExternalAttributes(const TArray<TSoftClassPtr<UAttributeSet>>& AttributeSets)
{
	for (const auto& AttributeSetClass : AttributeSets)
		if (AttributeSetClass.IsValid())
			GrantedAttributes.Add(FAbilitySet_AttributeSet { AttributeSetClass });
}

