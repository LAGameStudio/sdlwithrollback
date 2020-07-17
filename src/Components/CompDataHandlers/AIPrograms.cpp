#include "Components/AIPrograms/Defend.h"
#include "Components/GameActor.h"
#include "Components/Transform.h"

#include "StateMachine/ActionTimer.h"

#include "GameManagement.h"

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
    return InputState::NONE;
  }
}
