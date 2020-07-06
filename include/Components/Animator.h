#pragma once
#include "AssetManagement/Animation.h"
#include "StateMachine/IAction.h"

class Animator : public IComponent
{
public:
  Animator(std::shared_ptr<Entity> owner);

  void SetAnimations(AnimationCollection* animations);

  // Setter function
  Animation* Play(const std::string& name, bool isLooped, bool horizontalFlip, float speed = 1.0f);

  void ChangeListener(IAnimatorListener* listener) { _listener = listener; }

  Animation& GetCurrentAnimation() { return _currentAnimation->second; }

  //!
  IAnimatorListener* GetListener() { return _listener; }
  //!
  AnimationCollection* AnimationLib() {return _animations; }
  // STATE VARIABLES
  //!
  bool playing;
  //!
  bool looping;
  //!
  float accumulatedTime;
  //!
  int frame;
  //!
  std::string currentAnimationName;
  //! multiplier for speed of animation to play at
  float playSpeed = 1.0f;
  

  friend std::ostream& operator<<(std::ostream& os, const Animator& animator);
  friend std::istream& operator>>(std::istream& is, Animator& animator);

protected:
  //! Things that need to know when an animation is done
  IAnimatorListener* _listener;
  //! All animations registered to this animator
  AnimationCollection* _animations = nullptr;
  //!
  std::unordered_map<std::string, Animation>::iterator _currentAnimation;

};

template <> struct ComponentInitParams<Animator>
{
  AnimationCollection* collection;
  std::string name;
  bool isLooped;
  bool horizontalFlip;
  float speed = 1.0f;
  static void Init(Animator& component, const ComponentInitParams<Animator>& params)
  {
    component.SetAnimations(params.collection);
    component.Play(params.name, params.isLooped, params.horizontalFlip, params.speed);
  }
};
