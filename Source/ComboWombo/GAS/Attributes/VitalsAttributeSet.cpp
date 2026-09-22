#include "GAS/Attributes/VitalsAttributeSet.h"

UVitalsAttributeSet::UVitalsAttributeSet()
	: Health(100.0f), MaxHealth(100.0f)
{
}

void UVitalsAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UVitalsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UVitalsAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	else if (Attribute == GetMaxHealthAttribute())
		NewValue = FMath::Max(NewValue, 1.f);
	else if (Attribute == GetPoiseAttribute())
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxPoise());
	else if (Attribute == GetMaxPoiseAttribute())
		NewValue = FMath::Max(NewValue, 1.f);
	
}
