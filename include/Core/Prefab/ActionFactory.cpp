#include "Core/Prefab/ActionFactory.h"

#include "Components/ActionComponents.h"
#include "Components/GameActor.h"
#include "Components/Animator.h"
#include "Components/StateComponents/AttackStateComponent.h"
#include "Components/StateComponents/HitStateComponent.h"

#include "StateMachine/ActionUtil.h"

#include "GameManagement.h"

//! Define action decided list
std::vector<Entity*> ActionFactory::_actionDecided;

void ActionFactory::SetKnockdownAirborne(Entity* entity, StateComponent* state)
{
  // since this will set to new state, remove input listener flag
  _actionDecided.push_back(entity);

  GameManager::Get().TriggerEndOfFrame([entity, state]()
    {
      RemoveTransitionComponents(entity);
      DisableAbility(entity);

      state->actionState = ActionState::HITSTUN;

      // Always reset action complete flag on new action
      entity->GetComponent<GameActor>()->actionTimerComplete = false;

      // set up knockdown air action
      entity->AddComponent<AnimatedActionComponent>({ state->onLeftSide, false, true, 1.0f, "Knockdown_Air" });

      // set up movement action for knockback
      entity->AddComponent<MovingActionComponent>();
      entity->GetComponent<MovingActionComponent>()->velocity = state->hitData.knockback;

      // set empty component for follow up action
      entity->AddComponent<TransitionToKnockdownGroundOTG>();;

      // add hittable because you can still be hit while in the air
      entity->AddComponent<HittableState>();
      entity->GetComponent<HittableState>()->canBlock = false;
      entity->GetComponent<HittableState>()->inKnockdown = true;

      entity->AddComponent<ReceivedDamageAction>();
      entity->GetComponent<ReceivedDamageAction>()->damageAmount = state->hitData.damage;
      entity->GetComponent<ReceivedDamageAction>()->isBlocking = false;

      entity->AddComponent<EnactActionComponent>();

      // in case we still have a grapple state attached
      entity->RemoveComponent<ReceivedGrappleAction>();
    });
}

void ActionFactory::SetKnockdownGroundOTG(Entity* entity, StateComponent* state)
{
  // since this will set to new state, remove input listener flag
  _actionDecided.push_back(entity);

  GameManager::Get().TriggerEndOfFrame([entity, state]()
    {
      RemoveTransitionComponents(entity);
      DisableAbility(entity);

      state->actionState = ActionState::HITSTUN;

      // Always reset action complete flag on new action
      entity->GetComponent<GameActor>()->actionTimerComplete = false;

      // set up knockdown air action
      entity->AddComponent<AnimatedActionComponent>({ state->onLeftSide, false, false, 1.0f, "Knockdown_HitGround" });

      // set up movement action for knockback
      entity->AddComponent<MovingActionComponent>();
      entity->GetComponent<MovingActionComponent>()->velocity = Vector2<float>::Zero;

      // freeze in this state until animation is done
      entity->AddComponent<WaitForAnimationComplete>();

      // set empty component for follow up action
      entity->AddComponent<TransitionToKnockdownGround>();

      // add hittable because you will be able to OTG during this period
      entity->AddComponent<HittableState>();
      entity->GetComponent<HittableState>()->canBlock = false;
      entity->GetComponent<HittableState>()->inKnockdown = true;

      entity->AddComponent<EnactActionComponent>();

      // remove stuff from last state
      entity->RemoveComponent<TransitionToKnockdownGroundOTG>();
      entity->RemoveComponent<ReceivedDamageAction>();
      entity->RemoveComponent<ReceivedGrappleAction>();
    });
}

void ActionFactory::SetKnockdownGroundInvincible(Entity* entity, StateComponent* state)
{
  // since this will set to new state, remove input listener flag
  _actionDecided.push_back(entity);

  GameManager::Get().TriggerEndOfFrame([entity, state]()
    {
      RemoveTransitionComponents(entity);
      DisableAbility(entity);

      state->actionState = ActionState::NONE;

      // Always reset action complete flag on new action
      entity->GetComponent<GameActor>()->actionTimerComplete = false;

      // set up knockdown air action
      entity->AddComponent<AnimatedActionComponent>({ state->onLeftSide, false, false, 1.0f, "Knockdown_OnGround" });

      // set up movement action for knockback
      entity->AddComponent<MovingActionComponent>();
      entity->GetComponent<MovingActionComponent>()->velocity = Vector2<float>::Zero;

      // freeze in this state until animation is done
      entity->AddComponent<WaitForAnimationComplete>();

      // set empty component for follow up action
      entity->AddComponent<TransitionToNeutral>();

      entity->AddComponent<EnactActionComponent>();

      // remove attack components cause am in hitstun
      entity->RemoveComponent<AbleToAttackState>();
      entity->RemoveComponent<AbleToSpecialAttackState>();

      // not hittable during this period
      entity->RemoveComponent<HittableState>();

      entity->RemoveComponent<TransitionToKnockdownGround>();
    });
}

void ActionFactory::SetBlockStunAction(Entity* entity, StateComponent* state, bool crouching)
{
  // since this will set to new state, remove input listener flag
  _actionDecided.push_back(entity);

  GameManager::Get().TriggerEndOfFrame([entity, state, crouching]()
    {
      RemoveTransitionComponents(entity);
      DisableAbility(entity);

      state->actionState = ActionState::BLOCKSTUN;

      // Always reset action complete flag on new action
      entity->GetComponent<GameActor>()->actionTimerComplete = false;

      // set up knockdown air action
      entity->AddComponent<AnimatedActionComponent>({ state->onLeftSide, false, true, 1.0f, crouching ? "BlockLow" : "BlockMid" });

      if (HasState(state->collision, CollisionSide::DOWN))
      {
        // set up movement action for stopping movement
        entity->AddComponent<MovingActionComponent>();
        entity->GetComponent<MovingActionComponent>()->velocity = Vector2<float>::Zero;
      }

      // freeze in this state 
      entity->AddComponent<TimedActionComponent>();
      entity->GetComponent<TimedActionComponent>()->totalFrames = state->hitData.framesInStunBlock;

      // set empty component for follow up action
      entity->AddComponent<TransitionToNeutral>();

      // add states for potential inputs
      entity->AddComponent<HittableState>();
      entity->GetComponent<HittableState>()->canBlock = true;
      entity->GetComponent<HittableState>()->inKnockdown = false;

      entity->AddComponent<ReceivedDamageAction>();
      entity->GetComponent<ReceivedDamageAction>()->damageAmount = 0;
      entity->GetComponent<ReceivedDamageAction>()->isBlocking = true;

      entity->AddComponent<EnactActionComponent>();
    });
}

void ActionFactory::SetHitStunAction(Entity* entity, StateComponent* state, bool crouching)
{
  // since this will set to new state, remove input listener flag
  _actionDecided.push_back(entity);

  GameManager::Get().TriggerEndOfFrame([entity, state, crouching]()
    {
      RemoveTransitionComponents(entity);
      DisableAbility(entity);

      state->actionState = ActionState::HITSTUN;

      // Always reset action complete flag on new action
      entity->GetComponent<GameActor>()->actionTimerComplete = false;

      std::string hitstunAnim = "LightHitstun";
      if (state->hitData.framesInStunHit > 10) hitstunAnim = "MedHitstun";
      if (state->hitData.framesInStunHit > 15) hitstunAnim = "HeavyHitstun";

      // set up animation
      entity->AddComponent<AnimatedActionComponent>({ state->onLeftSide, false, true, 1.0f, (crouching && state->hitData.framesInStunHit < 16) ? "CrouchingHitstun" : hitstunAnim });

      // set up movement action for knockback
      entity->AddComponent<MovingActionComponent>();
      entity->GetComponent<MovingActionComponent>()->velocity = state->hitData.knockback;

      // freeze in this state 
      entity->AddComponent<TimedActionComponent>();
      entity->GetComponent<TimedActionComponent>()->totalFrames = state->hitData.framesInStunHit;

      // set empty component for follow up action
      entity->AddComponent<TransitionToNeutral>();

      entity->AddComponent<ReceivedDamageAction>();
      entity->GetComponent<ReceivedDamageAction>()->damageAmount = state->hitData.damage;
      entity->GetComponent<ReceivedDamageAction>()->isBlocking = false;

      // add states for potential inputs
      entity->AddComponent<HittableState>();
      entity->GetComponent<HittableState>()->canBlock = false;
      entity->GetComponent<HittableState>()->inKnockdown = false;

      entity->AddComponent<EnactActionComponent>();
    });
}

void ActionFactory::SetGrappledAction(Entity* entity, StateComponent* state)
{
  // since this will set to new state, remove input listener flag
  _actionDecided.push_back(entity);

  GameManager::Get().TriggerEndOfFrame([entity, state]()
    {
      RemoveTransitionComponents(entity);
      DisableAbility(entity);

      // Always reset action complete flag on new action
      entity->GetComponent<GameActor>()->actionTimerComplete = false;

      // set up animation
      entity->AddComponent<AnimatedActionComponent>({ state->onLeftSide, false, true, 1.0f, "HeavyHitstun" });

      // set up movement action for stopping movement
      //entity->AddComponent<MovingActionComponent>();
      //entity->GetComponent<MovingActionComponent>()->velocity = Vector2<float>::Zero;

      entity->AddComponent<ReceivedGrappleAction>();
      entity->GetComponent<ReceivedGrappleAction>()->damageAndKnockbackDelay = state->hitData.activeFrames;
      entity->GetComponent<ReceivedGrappleAction>()->damageAmount = state->hitData.damage;

      // freeze in this state til throw is initiated
      entity->AddComponent<TimedActionComponent>();
      entity->GetComponent<TimedActionComponent>()->totalFrames = state->hitData.activeFrames;

      entity->AddComponent<EnactActionComponent>();
    });
}


void ActionFactory::SetAttackAction(Entity* entity, StateComponent* state, const std::string& attackName, ActionState actionType)
{
  // since this will set to new state, remove input listener flag
  _actionDecided.push_back(entity);

  GameManager::Get().TriggerEndOfFrame([entity, state, attackName, actionType]()
    {
      RemoveTransitionComponents(entity);
      DisableAbility(entity);

      // Always reset action complete flag on new action
      entity->GetComponent<GameActor>()->actionTimerComplete = false;

      // set up animation
      entity->AddComponent<AnimatedActionComponent>({ state->onLeftSide, false, true, 1.0f, attackName });

      // since this is an animation locked attack
      entity->AddComponent<WaitForAnimationComplete>();

      entity->AddComponent<AttackActionComponent>();
      entity->GetComponent<AttackActionComponent>()->type = actionType;

      if (HasState(state->collision, CollisionSide::DOWN))
      {
        // set up movement action for stopping movement
        entity->AddComponent<MovingActionComponent>();
        entity->GetComponent<MovingActionComponent>()->velocity = Vector2<float>::Zero;
      }

      // add states for potential inputs
      entity->AddComponent<HittableState>();
      entity->GetComponent<HittableState>()->canBlock = false;
      entity->GetComponent<HittableState>()->inKnockdown = false;

      // set empty component for follow up action
      entity->AddComponent<TransitionToNeutral>();

      // add cancels here
      entity->AddComponent<CancelOnHitGround>();

      entity->AddComponent<EnactActionComponent>();
    });
}

void ActionFactory::SetDashAction(Entity* entity, StateComponent* state, Animator* animator, bool dashDirectionForward)
{
  // since this will set to new state, remove input listener flag
  _actionDecided.push_back(entity);

  GameManager::Get().TriggerEndOfFrame([entity, state, animator, dashDirectionForward]()
    {
      RemoveTransitionComponents(entity);
      DisableAbility(entity);

      // Always reset action complete flag on new action
      entity->GetComponent<GameActor>()->actionTimerComplete = false;

      std::string animationName = dashDirectionForward ? "ForwardDash" : "BackDash";
      // set up animation
      float dashPlaySpeed = static_cast<float>(animator->AnimationLib()->GetAnimation(animationName)->GetFrameCount()) / static_cast<float>(ActionParams::nDashFrames);
      entity->AddComponent<AnimatedActionComponent>({ state->onLeftSide, false, false, dashPlaySpeed, animationName });

      entity->AddComponent<DashingAction>();
      entity->GetComponent<DashingAction>()->dashSpeed = ActionParams::baseWalkSpeed * 1.5f;
      if ((state->onLeftSide && !dashDirectionForward) || (!state->onLeftSide && dashDirectionForward))
        entity->GetComponent<DashingAction>()->dashSpeed *= -1.0f;

      // set up the end timer
      entity->AddComponent<TimedActionComponent>();
      entity->GetComponent<TimedActionComponent>()->totalFrames = ActionParams::nDashFrames;


      // add states for potential inputs
      entity->AddComponent<HittableState>();
      entity->GetComponent<HittableState>()->canBlock = false;
      entity->GetComponent<HittableState>()->inKnockdown = false;

      // set empty component for follow up action
      entity->AddComponent<TransitionToNeutral>();

      entity->AddComponent<EnactActionComponent>();
    });
}

void ActionFactory::GoToNeutralAction(Entity* entity, StateComponent* state)
{
  // since this will set to new state, remove input listener flag
  _actionDecided.push_back(entity);

  GameManager::Get().TriggerEndOfFrame([entity, state]()
    {
      ResetActionComponents(entity);
      // Always reset action complete flag on new action
      entity->GetComponent<GameActor>()->actionTimerComplete = false;

      entity->AddComponent<EnactActionComponent>();

      if (entity->GetComponent<CrouchingAction>())
        entity->AddComponent<AnimatedActionComponent>({ state->onLeftSide, true, false, 1.0f, "Crouch" });
      else
        entity->AddComponent<AnimatedActionComponent>({ state->onLeftSide, true, false, 1.0f, "Idle" });

      // set up movement action for stopping movement
      entity->AddComponent<MovingActionComponent>();
      entity->GetComponent<MovingActionComponent>()->velocity = Vector2<float>::Zero;

      // add states for potential outside influence
      entity->AddComponent<HittableState>();
      entity->GetComponent<HittableState>()->canBlock = true;
      entity->GetComponent<HittableState>()->inKnockdown = false;

      // in neutral, all of these options are available, so actions can be accessed via inputs
      EnableAbility(entity);

      entity->RemoveComponents<DashingAction>();

      state->actionState = ActionState::NONE;
    });
}

void ActionFactory::ResetActionComponents(Entity* entity)
{
  // remove all state dependent components (hopefully this doesn't fuck performance)
  RemoveTransitionComponents(entity);

  entity->RemoveComponent<CancelOnHitGround>();
  entity->RemoveComponent<CancelOnDash>();
  entity->RemoveComponent<CancelOnJump>();

  entity->RemoveComponent<AttackActionComponent>();
  entity->RemoveComponent<GrappleActionComponent>();
  entity->RemoveComponent<MovingActionComponent>();
  entity->RemoveComponent<ReceivedDamageAction>();
  entity->RemoveComponent<ReceivedGrappleAction>();
  entity->RemoveComponent<DashingAction>();

  entity->AddComponent<InputListenerComponent>();
  
}

void ActionFactory::RemoveTransitionComponents(Entity* entity)
{
  entity->RemoveComponent<TransitionToKnockdownGround>();
  entity->RemoveComponent<TransitionToKnockdownGroundOTG>();
  entity->RemoveComponent<TransitionToNeutral>();
  entity->RemoveComponent<TransitionToCrouching>();

  entity->RemoveComponent<TimedActionComponent>();
  entity->RemoveComponent<WaitForAnimationComplete>();
  entity->RemoveComponent<WaitingForJumpAirborne>();
  entity->RemoveComponent<MovingActionComponent>();

  entity->RemoveComponent<AttackStateComponent>();
  entity->RemoveComponent<HitStateComponent>();

  // remove all action state components
  //entity->RemoveComponents<DashingAction, JumpingAction, CrouchingAction>();
}


void ActionFactory::DisableAbility(Entity* entity)
{
  entity->RemoveComponents<AbleToAttackState, AbleToSpecialAttackState, AbleToDash, AbleToJump, AbleToWalk, AbleToCrouch>();
}

void ActionFactory::EnableAbility(Entity* entity)
{
  entity->AddComponents<AbleToAttackState, AbleToSpecialAttackState, AbleToDash, AbleToJump, AbleToWalk, AbleToCrouch>();
  entity->GetComponent<GameActor>()->newInputs = true;
}

void ActionFactory::DisableActionListenerForEntities()
{
  for (Entity* entity : _actionDecided)
  {
    entity->RemoveComponent<InputListenerComponent>();
  }
  _actionDecided.clear();
}
