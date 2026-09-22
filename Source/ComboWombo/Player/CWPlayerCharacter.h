#pragma once

#include "CoreMinimal.h"
#include "Characters/CWCharacterBase.h"
#include "Logging/LogMacros.h"
#include "CWPlayerCharacter.generated.h"

class UPlayerPerceptionComponent;
class UPlayerTargetingComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UCWAbilitySystemComponent;
class UVitalsAttributeSet;
class UMovementAttributeSet;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ACWPlayerCharacter : public ACWCharacterBase
{
	GENERATED_BODY()

public:
	/** Constructor */
	ACWPlayerCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void UnPossessed() override;

	// Camera-facing strafing while moving; free camera orbit while idle.
	UFUNCTION(BlueprintCallable, Category="Camera")
	void SetCameraControlledStrafing(bool bEnabled);
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Activates attack ability */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* SprintAction;
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Clears movement intent when the movement action ends. */
	void ClearCachedMovementInput();

	/** Normalized world-space movement intent; zero means no active direction. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category="Input")
	FVector CachedMovementInputDirection = FVector::ZeroVector;

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Returns active movement intent, or zero when no direction is cached. */
	UFUNCTION(BlueprintPure, Category="Input")
	FVector GetCachedMovementInputDirection() const { return CachedMovementInputDirection; }

	/** Returns movement intent in world space, falling back to the character's forward vector. */
	UFUNCTION(BlueprintPure, Category="Input")
	FVector GetAttackInputDirection() const;

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

protected:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerPerceptionComponent> PlayerPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPlayerTargetingComponent> PlayerTargetingComponent;
	
	UFUNCTION(BlueprintNativeEvent, Category="Input")
	void OnAttackInput();

	UFUNCTION(BlueprintNativeEvent, Category="Input")
	void OnJumpInput();

	UFUNCTION(BlueprintNativeEvent)
	void UpdateSprintState();

	UFUNCTION(BlueprintNativeEvent, Category="Input")
	bool IsSprinting() const;	
	
	UPROPERTY(BlueprintReadWrite)
	bool bWantsToSprint = false;
	
public: // ICharacterPerception
	virtual TArray<AActor*> GetEnemies_Implementation() const override;
	
public: // ICombatant
	virtual void OnKilledActor_Implementation(AActor* KilledActor, const FReceivedHitData& LastHit) override;
	virtual void OnStaggeredActor_Implementation(AActor* StaggeredActor, const FReceivedHitData& LastHit) override;
	virtual AActor* GetBestTarget_Implementation() const override;
	virtual void OnAttackStarted_Implementation() override;
	virtual void OnAttackEnded_Implementation() override;
	
private:
	void OnRequestSprint();
	void OnStopSprint();
};

