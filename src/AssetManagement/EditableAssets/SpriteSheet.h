#pragma once
#include "IJsonLoadable.h"
#include "Core/Math/Vector2.h"

struct SpriteSheet : public IJsonLoadable
{
  SpriteSheet() = default;
  SpriteSheet(const char* src, int rows, int columns);
  SpriteSheet(const char* src, int rows, int columns, bool dontLoad) : src(src), rows(rows), columns(columns) {}

  void GenerateSheetInfo();
  //!
  std::string src;
  //!
  Vector2<int> frameSize;
  Vector2<int> sheetSize;
  //!
  int rows, columns;

  virtual void Load(const Json::Value& json) override;
  virtual void Write(Json::Value& json) const override;
  virtual void DisplayInEditor() override;
};
