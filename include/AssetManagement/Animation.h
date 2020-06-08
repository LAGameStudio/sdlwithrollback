#pragma once
#include "Components/Transform.h"
#include "Components/StateComponent.h"
#include "AssetManagement/BlitOperation.h"
#include "ComponentConst.h"
#include "ResourceManager.h"

#include <functional>
#include <unordered_map>

//!
const float secPerFrame = 1.0f / 60.0f;
const float gameFramePerAnimationFrame = (1.0f / secPerFrame) / animation_fps;

class Animation;

//______________________________________________________________________________
class AnimationEvent
{
public:
  AnimationEvent(int startFrame, int duration, std::function<void(Transform*, StateComponent*)> onTriggerCallback, std::vector<std::function<void(Transform*, StateComponent*)>> update, std::function<void(Transform*)> onEndCallback) :
    _frame(startFrame), _duration(duration), _onTrigger(onTriggerCallback), _updates(update), _onEnd(onEndCallback) {}

  void TriggerEvent(Transform* trans, StateComponent* state) { _onTrigger(trans, state); }
  void UpdateEvent(int frame, Transform* trans, StateComponent* state) { _updates[frame - _frame - 1](trans, state); }
  void EndEvent(Transform* trans) { _onEnd(trans); }
  int GetEndFrame() { return _frame + _duration; }

private:
  //! Delete copy constructor because it will only be used in the animation
  AnimationEvent(const AnimationEvent&) = delete;
  AnimationEvent operator=(AnimationEvent&) = delete;
  //! Frame this event will be called on
  int _frame;
  int _duration;
  //!
  std::function<void(Transform*, StateComponent*)> _onTrigger;
  std::vector<std::function<void(Transform*, StateComponent*)>> _updates;
  std::function<void(Transform*)> _onEnd;
};

typedef std::tuple<int, int, std::function<void(Transform*, StateComponent*)>, std::vector<std::function<void(Transform*, StateComponent*)>>, std::function<void(Transform*)>> EventInitParams;

//______________________________________________________________________________
class Animation
{
public:
  Animation(const SpriteSheet& sheet, int startIndexOnSheet, int frames, AnchorPoint anchor);

  EventInitParams GenerateAttackEvent(const char* hitboxesSheet, FrameData frameData);

  EventInitParams GenerateAttackEvent(const std::vector<Rect<double>>& hitboxInfo, FrameData frameData);

  //! Translates anim frame to the frame on spritesheet
  SDL_Rect GetFrameSrcRect(int animFrame) const;

  const int GetFrameCount() const { return static_cast<int>(_animFrameToSheetFrame.size()); }

  Vector2<int> GetFrameWH() const { return _spriteSheet.frameSize; }
  //!
  template <typename Texture>
  Resource<Texture>& GetSheetTexture() const;
  //! Gets first non-transparent pixel from the top left and bottom left
  Vector2<int> FindAnchorPoint(AnchorPoint anchorType, bool fromFirstFrame) const;
  //!
  std::pair<AnchorPoint, Vector2<int>> const& GetMainAnchor() const { return _anchorPoint; }

  int GetFlipMargin() const { return _spriteSheet.frameSize.x - _lMargin; }

  struct ImGuiDisplayParams
  {
    void* ptr;
    Vector2<int> displaySize;
    Vector2<float> uv0;
    Vector2<float> uv1;

  };
  ImGuiDisplayParams GetUVCoordsForFrame(int displayHeight, int animFrame);

protected:
  //!
  SpriteSheet _spriteSheet;
  //!
  int _frames, _startIdx;
  //! stores the bottom left and top left reference pixels
  //Vector2<int> _anchorPoints[(const int)AnchorPoint::Size];
  //!
  std::vector<int> _animFrameToSheetFrame;
  
  //!
  std::pair<AnchorPoint, Vector2<int>> _anchorPoint;
  //! finding margin from the bottom right now
  int _lMargin, _rMargin, _tMargin;

private:
  //! Gets a series of rectangles from a sprite sheet auxiliary file
  std::vector<Rect<double>> GetHitboxesFromFile(const char* hitboxesSheet);

};

//______________________________________________________________________________
template <typename Texture>
inline Resource<Texture>& Animation::GetSheetTexture() const
{
  return ResourceManager::Get().GetAsset<Texture>(_spriteSheet.src);
}

typedef std::unordered_map<int, AnimationEvent> EventList;

//______________________________________________________________________________
class AnimationCollection
{
public:
  AnimationCollection() = default;
  void RegisterAnimation(const std::string& animationName, const SpriteSheet& sheet, int startIndexOnSheet, int frames, AnchorPoint anchor);
  void SetHitboxEvents(const std::string& animationName, const char* hitboxesSheet, FrameData frameData);


  Vector2<int> GetRenderOffset(const std::string& animationName, bool flipped, int transformWidth) const;
  //! Getters
  Animation* GetAnimation(const std::string& name)
  {
    if(_animations.find(name) == _animations.end())
      return nullptr;
    return &_animations.find(name)->second;
  }
  //!
  EventList* GetEventList(const std::string& name)
  {
    if(_events.find(name) == _events.end())
      return nullptr;
    return _events.find(name)->second.get();
  }

  std::unordered_map<std::string, Animation>::iterator GetAnimationIt(const std::string& name)
  {
    return _animations.find(name);
  }

  std::unordered_map<std::string, Animation>::iterator GetEnd()
  {
    return _animations.end();
  }

  bool IsValid(const std::unordered_map<std::string, Animation>::iterator& it)
  {
    return it != _animations.end();
  }

private:
  //! Map of animations name to animation object
  std::unordered_map<std::string, Animation> _animations;
  //! Map of frame starts for events to the event that should be triggered
  std::unordered_map<std::string, std::shared_ptr<EventList>> _events;
  //!
  Vector2<int> _anchorPoint[(const int)AnchorPoint::Size];
  Rect<int> _anchorRect;

  //! use the first sprite sheet in as anchor point reference
  bool _useFirstSprite = false;

};
