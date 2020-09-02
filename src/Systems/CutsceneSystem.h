#pragma once
#include "Core/ECS/ISystem.h"
#include "Components/Animator.h"

class CutsceneSystem : public ISystem<CutsceneActor, Animator, RenderComponent<RenderType>, RenderProperties>
{
public:
  static void DoTick(float dt)
  {
    bool scenePlaying = false;
    for(const EntityID& entity : Registered)
    {
      CutsceneActor& actor = ComponentArray<CutsceneActor>::Get().GetComponent(entity);
      Animator& animator = ComponentArray<Animator>::Get().GetComponent(entity);
      RenderComponent<RenderType>& renderer = ComponentArray<RenderComponent<RenderType>>::Get().GetComponent(entity);
      RenderProperties& properties = ComponentArray<RenderProperties>::Get().GetComponent(entity);

      CutsceneAction* action = nullptr;
      //check if actor has started yet
      if(!actor.started)
      {
        action = actor.ActionListPop();
        if(action)
          action->Begin(&animator, &renderer, &properties);
      }
      else
      {
        action = actor.currentAction;
      }
      if(action)
      {
        // at least one has a current action, so scene is playing
        scenePlaying = true;

        if(action->isWaiting)
        {
          for(const EntityID& other : Registered)
          {
            if(other != entity)
            {
              if(action->CheckEndConditions(&ComponentArray<CutsceneActor>::Get().GetComponent(other)))
              {
                action->OnComplete();
                action = actor.ActionListPop();
                if(action)
                  action->Begin(&animator, &renderer, &properties);
              }
            }
          }
        }
        else
        {
          if(action->CheckEndConditions())
          {
            action->OnComplete();
            action = actor.ActionListPop();
            if(action)
              action->Begin(&animator, &renderer, &properties);
          }
        }
      }
    }

    // when scene is finished move to the results screen
    if(!scenePlaying)
    {
      GameManager::Get().RequestSceneChange(SceneType::RESULTS);
    }
  }
};

//! prevents movement on ground
class CutsceneMovementSystem : public ISystem<Rigidbody>
{
public:
  static void DoTick(float dt)
  {
    for(const EntityID& entity : Registered)
    {
      Rigidbody& rb = ComponentArray<Rigidbody>::Get().GetComponent(entity);
      if(rb.lastCollisionSide == CollisionSide::DOWN)
      {
        rb.velocity = Vector2<float>::Zero;
      }
    }
  }
};