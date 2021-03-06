#include "Systems/WallPush/WallPushSystem.h"
#include "Managers/GameManagement.h"

#include "Core/Utility/DeferGuard.h"

void WallPushSystem::DoTick(float dt)
{
  PROFILE_FUNCTION();
  DeferGuard guard;
  for (const EntityID& entity : Registered)
  {
    Rigidbody& rigidbody = ComponentArray<Rigidbody>::Get().GetComponent(entity);
    WallPushComponent& push = ComponentArray<WallPushComponent>::Get().GetComponent(entity);
    Transform& transform = ComponentArray<Transform>::Get().GetComponent(entity);

    rigidbody.velocity.x = push.velocity;
    push.amountPushed += push.velocity * dt;
    if (std::fabs(push.amountPushed) >= std::fabs(push.pushAmount))
    {
      rigidbody.velocity.x = 0;
      RunOnDeferGuardDestroy(entity, GameManager::Get().GetEntityByID(entity)->RemoveComponent<WallPushComponent>());
    }
  }
}
