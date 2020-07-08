#pragma once
#include "Systems/ISystem.h"

#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/Rigidbody.h"
#include "Components/Hurtbox.h"
#include "Components/Hitbox.h"

class MoveSystemCamera : public ISystem<Transform, Camera>
{
public:
  static void DoTick(float dt)
  {
    for (auto tuple : Tuples)
    {
      Transform* transform = std::get<Transform*>(tuple.second);
      Camera* camera = std::get<Camera*>(tuple.second);

      camera->rect.x = static_cast<int>(std::floor(transform->position.x));
      camera->rect.y = static_cast<int>(std::floor(transform->position.y));

      // also update camera matrix here
      const Vector2<float> origin(m_nativeWidth / 2.0f, m_nativeHeight / 2.0f);
      camera->matrix =
        //Mat4::Translation(-transform->position.x, -transform->position.y, 0) *
        Mat4::Translation(-origin.x, -origin.y, 0) *
        Mat4::Scale(camera->zoom, camera->zoom, 1.0f) *
        Mat4::RotationZAxis(transform->rotation.x) * // should be z here
        Mat4::Translation(origin.x, origin.y, 0);
    }
  }
};

class MoveSystemPhysCollider : public ISystem<Transform, DynamicCollider>
{
public:
  static void DoTick(float dt)
  {
    for (auto tuple : Tuples)
    {
      Transform* transform = std::get<Transform*>(tuple.second);
      DynamicCollider* rect = std::get<DynamicCollider*>(tuple.second);

      rect->MoveToTransform(*transform);
      
    }
  }
};

class MoveSystemHurtbox : public ISystem<Transform, Hurtbox>
{
public:
  static void DoTick(float dt)
  {
    for (auto tuple : Tuples)
    {
      Transform* transform = std::get<Transform*>(tuple.second);
      Hurtbox* hurtbox = std::get<Hurtbox*>(tuple.second);

      hurtbox->MoveToTransform(*transform);
      
    }
  }
};

class MoveSystemHitbox : public ISystem<Transform, Hitbox>
{
public:
  static void DoTick(float dt)
  {
    for (auto tuple : Tuples)
    {
      Transform* transform = std::get<Transform*>(tuple.second);
      Hitbox* hitbox = std::get<Hitbox*>(tuple.second);

      if (hitbox->travelWithTransform)
        hitbox->MoveToTransform(*transform);
    }
  }
};

class MoveSystem : public ISystem<Transform>
{
public:
  static void DoTick(float dt)
  {
    MoveSystemCamera::DoTick(dt);
    MoveSystemPhysCollider::DoTick(dt);
    MoveSystemHurtbox::DoTick(dt);
    MoveSystemHitbox::DoTick(dt);
  }
};
