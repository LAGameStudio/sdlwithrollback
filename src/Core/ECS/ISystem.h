#pragma once
#include "Core/ECS/Entity.h"
#include "Core/ECS/ComponentTraits.h"

#include <cstdint>
#include <map>

#include <set>

template <typename ... T>
struct Requires {};

template <typename T>
struct Requires<T>
{
  static bool MatchesSignature(EntityID entity)
  {
    return (EntityManager::Get().GetSignature(entity) & ComponentTraits<T>::GetSignature()) == ComponentTraits<T>::GetSignature();
  }
};

template <typename T, typename ... Rest>
struct Requires<T, Rest...>
{
  static bool MatchesSignature(EntityID entity)
  {
    auto combinedSignature = ComponentTraits<T>::GetSignature() | (ComponentTraits<Rest>::GetSignature() | ...);
    return (EntityManager::Get().GetSignature(entity) & combinedSignature) == combinedSignature;
  }

  static bool HasOneRequirement(EntityID entity)
  {
    auto combinedSignature = ComponentTraits<T>::GetSignature() | (ComponentTraits<Rest>::GetSignature() | ...);
    return (EntityManager::Get().GetSignature(entity) & combinedSignature) != 0;
  }
};

template <typename ... T>
class ISystem
{
public:
  //! 
  static void Check(Entity* entity)
  {
    if(Req::MatchesSignature(entity->GetID()))
    {
      Registered.insert(entity->GetID());
    }
    else
    {
      Registered.erase(entity->GetID());
    }
  }

  //!
  //static std::map<int, std::tuple<T&...>> Tuples;

    //! Set of registered entities
  static std::set<EntityID> Registered;

protected:

  //! Required components
  using Req = Requires<T...>;

};

//template <typename ... T>
//std::map<int, std::tuple<T&...>> ISystem<T...>::Tuples;

template <typename ... T>
std::set<EntityID> ISystem<T...>::Registered;


//_________________________________________________________________________
// Multi System facilitates interaction between multiple entities in a system

template <typename ... T>
struct SysComponents {};

template <typename Main, typename Sub>
class IMultiSystem;

template <typename ... Main, typename ... Sub>
class IMultiSystem<SysComponents<Main...>, SysComponents<Sub...>>
{
public:
  class MainSystem : public ISystem<Main...> {};

  class SubSystem : public ISystem<Sub...> {};

  static void Check(Entity* entity)
  {
    MainSystem::Check(entity);
    SubSystem::Check(entity);
  }
};
