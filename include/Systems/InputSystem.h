#pragma once
#include "Systems/ISystem.h"
#include "Components/GameActor.h"
#include "Components/Input.h"

#include "Components/Transform.h"
#include "Components/Rigidbody.h"

class InputSystem : public ISystem<KeyboardInputHandler, StateComponent, GameActor, Rigidbody, RectColliderD>
{
public:
  static void DoTick(float dt)
  {
    for(auto& tuple : Tuples)
    {
      KeyboardInputHandler* inputHandler = std::get<KeyboardInputHandler*>(tuple.second);
      GameActor* actor = std::get<GameActor*>(tuple.second);
      Rigidbody* rigidbody = std::get<Rigidbody*>(tuple.second);
      RectColliderD* collider = std::get<RectColliderD*>(tuple.second);
      StateComponent* state = std::get<StateComponent*>(tuple.second);

      const InputBuffer& unitInputState = inputHandler->CollectInputState();

      state->collision = rigidbody->_lastCollisionSide;

      for(auto& other : Tuples)
      {
        if(other != tuple)
        {
          state->onLeftSide = collider->rect.GetCenter().x < std::get<RectColliderD*>(other.second)->rect.GetCenter().x;
        }
      }
      actor->EvaluateInputContext(unitInputState, state, dt);

      rigidbody->elasticCollisions = actor->GetActionState() == ActionState::HITSTUN;
    }
  }
};