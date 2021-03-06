#include "Components/AIPrograms/Defend.h"

#include "Systems/TimerSystem/TimerContainer.h"
#include "Systems/TimerSystem/ActionTimer.h"

#include "Components/Transform.h"
#include "Components/StateComponents/HitStateComponent.h"
#include "Components/ActionComponents.h"

#include "Managers/GameManagement.h"

InputState DefendAI::Update(const Transform* t, const StateComponent* s)
{
  if(s->hitThisFrame)
  {
    if(s->onLeftSide)
      return InputState::LEFT;
    else 
      return InputState::RIGHT;
  }
  return InputState::NONE;
}

InputState DefendAI::Update(const InputState& input, const Transform* t, const StateComponent* s) 
{
  return input;
}

DefendAfter::DefendAfter()
{
  timerEntity = GameManager::Get().CreateEntity<TimerContainer>();
}

DefendAfter::~DefendAfter()
{
  GameManager::Get().DestroyEntity(timerEntity);
}

InputState DefendAfter::Update(const Transform* t, const StateComponent* s)
{
  // while waiting check for hit
  if(!wasHit)
  {
    if(s->hitThisFrame)
      wasHit = true;
    return InputState::NONE;
  }
  else if (isDefending)
  {
    InputState blockCommand = DefendAI::Update(t, s);
    if(s->hitThisFrame && blockCommand != InputState::NONE)
    {
      resetTimer->Cancel();
      resetTimer = std::shared_ptr<ActionTimer>(new SimpleActionTimer(
        [this]()
        { 
          isDefending = false;
          wasHit = false;
        },
        50
      ));
      timerEntity->GetComponent<TimerContainer>()->timings.push_back(resetTimer);
    }

    ActionState thisDefendingState = s->actionState;
    if (thisDefendingState == ActionState::NONE && lastDefendingState == ActionState::BLOCKSTUN)
    {
      // if not defending during this time, cancel the anim
      /*if (blockCommand == InputState::NONE)
      {
        resetTimer->Cancel();
        wasHit = false;
        isDefending = false;
      }*/
      blockCommand |= (InputState::BTN1 | InputState::DOWN);
    }
    lastDefendingState = thisDefendingState;

    return blockCommand;
  }
  else
  {
    // when it gets out of hit stun
    if(s->actionState != ActionState::HITSTUN)
    {
      resetTimer = std::shared_ptr<ActionTimer>(new SimpleActionTimer(
        [this]()
        { 
          isDefending = false;
          wasHit = false;
        },
        50
      ));
      timerEntity->GetComponent<TimerContainer>()->timings.push_back(resetTimer);
      isDefending = true;
      return DefendAI::Update(t, s);
    }
    else
    {
      // debug code
      /*if (s->hitThisFrame)
      {
        int realHitstunFrames = 0;
        int tFrames = 0;
        if (auto timerComp = t->GetComponent<TimedActionComponent>())
        {
          realHitstunFrames = timerComp->currFrame;
          tFrames = timerComp->totalFrames;
        }
        int remainingFrames = tFrames - realHitstunFrames;
      }*/
    }
    return InputState::NONE;
  }
}

InputState RepeatInputAI::Update(const Transform* t, const StateComponent* s)
{
  return input;
}

InputState RepeatInputAI::Update(const InputState& input, const Transform* t, const StateComponent* s)
{
  return input;
}
