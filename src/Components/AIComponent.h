#pragma once
#include "Core/ECS/IComponent.h"
#include "Components/InputHandlers/InputBuffer.h"
#include "Components/Transform.h"
#include "Components/StateComponent.h"

#include "Components/AIPrograms/IAIProgram.h"

// holds data relevant to AI decision making
class AIComponent : public IComponent
{
public:
  void UpdateMyState(const Transform* transform, const StateComponent* state);

  void UpdateFromOther(const Transform* transform, const StateComponent* state);

  std::shared_ptr<IAIProgram> program;

  InputState lastDecision = InputState::NONE;

};

inline void AIComponent::UpdateMyState(const Transform* transform, const StateComponent* state)
{
  lastDecision = InputState::NONE;
  if(program)
    lastDecision = program->Update(transform, state);
}

inline void AIComponent::UpdateFromOther(const Transform* transform, const StateComponent* state)
{
  if(program)
    lastDecision = program->Update(lastDecision, transform, state);
}
