#include "CWGameplayTags.h"

namespace CWGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack, "Ability.Attack")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Event_Combo_Request, "Ability.Attack.Event.Combo.Request", "Parameterless message sent to ASC by player input or AI decision to continue attack")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Event_Combo_Window, "Ability.Attack.Event.Combo.Window", "Parametetrized message sent by UAnimNotifyState_ComboWindow to tell active attack ability about active combo window with parameters")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Event_Combo_Advance, "Ability.Attack.Event.Combo.Advance", "Message sent by UAnimNotify_SendGameplayEvent to advance whatever commited combo continuation")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Event_MeleeSweep, "Ability.Attack.Event.MeleeSweep")
	UE_DEFINE_GAMEPLAY_TAG(Ability_HitReact, "Ability.HitReact")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Flinch, "Ability.Flinch")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Stagger, "Ability.Stagger")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Death, "Ability.Death")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Death_Event_Finished, "Ability.Death.Event.Finished")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump, "Ability.Jump")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Sprint, "Ability.Sprint")
	
	UE_DEFINE_GAMEPLAY_TAG(HitDirection_Front, "HitDirection.Front")
	UE_DEFINE_GAMEPLAY_TAG(HitDirection_Back, "HitDirection.Back")
	UE_DEFINE_GAMEPLAY_TAG(HitDirection_Right, "HitDirection.Right")
	UE_DEFINE_GAMEPLAY_TAG(HitDirection_Left, "HitDirection.Left")

	UE_DEFINE_GAMEPLAY_TAG(BodyRegion_Head, "BodyRegion.Head")
	UE_DEFINE_GAMEPLAY_TAG(BodyRegion_Body, "BodyRegion.Body")
	UE_DEFINE_GAMEPLAY_TAG(BodyRegion_Legs, "BodyRegion.Legs")

	UE_DEFINE_GAMEPLAY_TAG(State_Attacking, "State.Attacking")
	UE_DEFINE_GAMEPLAY_TAG(State_Sprinting, "State.Sprinting")
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead")

	UE_DEFINE_GAMEPLAY_TAG(Combat_Attack, "Combat.Attack")
	UE_DEFINE_GAMEPLAY_TAG(Combat_Attack_ComboStarter, "Combat.Attack.ComboStarter")
}