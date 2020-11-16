#pragma once
#include "IJsonLoadable.h"
#include "Core/Math/Vector2.h"
#include "DebugGUI/EditorString.h"

//______________________________________________________________________________
struct SpriteSheet : public IJsonLoadable
{
  SpriteSheet() = default;
  SpriteSheet(const char* src, int rows, int columns);
  SpriteSheet(const char* src, int rows, int columns, bool dontLoad) : src(src), rows(rows), columns(columns) {}

  void GenerateSheetInfo();
  void ShowSpriteSheetLines();
  //!
  EditorString src;
  //!
  Vector2<int> frameSize = Vector2<int>::Zero;
  Vector2<int> sheetSize = Vector2<int>::Zero;
  Vector2<int> offset = Vector2<int>::Zero;
  //!
  int rows = 0;
  int columns = 0;
  //!
  Vector2<double> renderScalingFactor = Vector2<double>(1.0, 1.0);

  virtual void Load(const Json::Value& json) override;
  virtual void Write(Json::Value& json) const override;
  virtual void DisplayInEditor() override;

};
