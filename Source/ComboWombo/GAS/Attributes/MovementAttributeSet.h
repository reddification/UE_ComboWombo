#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MovementAttributeSet.generated.h"

UCLASS()
class COMBOWOMBO_API UMovementAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMovementAttributeSet();
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS_BASIC(UMovementAttributeSet, MovementSpeed)
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData TurnRate;
	ATTRIBUTE_ACCESSORS_BASIC(UMovementAttributeSet, TurnRate)

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};
