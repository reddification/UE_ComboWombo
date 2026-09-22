#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "VitalsAttributeSet.generated.h"

UCLASS()
class COMBOWOMBO_API UVitalsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UVitalsAttributeSet();

	ATTRIBUTE_ACCESSORS_BASIC(UVitalsAttributeSet, Health)
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Health;
	
	ATTRIBUTE_ACCESSORS_BASIC(UVitalsAttributeSet, MaxHealth)
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Poise;
	ATTRIBUTE_ACCESSORS_BASIC(UVitalsAttributeSet, Poise);

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxPoise;
	ATTRIBUTE_ACCESSORS_BASIC(UVitalsAttributeSet, MaxPoise);
	
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData PoiseRecoveryRate;
	ATTRIBUTE_ACCESSORS_BASIC(UVitalsAttributeSet, PoiseRecoveryRate);
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};
