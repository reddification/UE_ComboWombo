#include "GameplayAbility_Attack_Player.h"
#include "Player/CWPlayerCharacter.h"

FVector UGameplayAbility_Attack_Player::GetAttackDirectionWithoutTarget_Implementation() const
{
	if (auto PlayerCharacter = Cast<ACWPlayerCharacter>(GetAvatarActorFromActorInfo()))
		return PlayerCharacter->GetCachedMovementInputDirection();
	
	return Super::GetAttackDirectionWithoutTarget_Implementation();
}
