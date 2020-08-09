#pragma once
#include "Core/Math/Vector2.h"

const float secPerFrame = 1.0f / 60.0f;

const float avg_animation_fps = 24.0f;
const float third_strike_fps = 16.0f;

const float animation_fps = third_strike_fps;

const int hitstopLight = 10;
const int hitstopMedium = 10;
const int hitstopHeavy = 10;

//! Set our preferred type (SDL or GL) to be rendered by the system
class GLTexture;
typedef GLTexture RenderType;


class ConstComponentIDGenerator
{
public:
  static int ID;
  static int NextID(bool& isInitialized, int value)
  {
    if (isInitialized)
      return value;

    isInitialized = true;
    return ID++;
  }
};
