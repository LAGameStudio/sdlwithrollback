#pragma once
#include "AssetManagement/Animation.h"

//! Easy access macro cause name is long :(
#define GAnimArchive AnimationCollectionManager::Get()

class AnimationCollectionManager
{
public:
  //! Static getter
  static AnimationCollectionManager& Get()
  {
    static AnimationCollectionManager manager;
    return manager;
  }
  //! Get collection ID from string name
  unsigned int GetCollectionID(std::string_view name);
  //! Get collection by ID
  AnimationCollection& GetCollection(unsigned int ID);
  //! Shortcut for getting animation data from collection
  Animation* GetAnimationData(unsigned int collectionID, std::string_view animationName);

  // remove this for a better way later
  //void ReloadRyuAnimation() {}

private:
  AnimationCollectionManager();

  // can all of these be made static constexpr if data is known at compile time?

  // for now, this is just a static value so keep it private
  void LoadCharacterCollection(const std::string& lookUpString,
    std::unordered_map<std::string, AnimationInfo>& normalAnimations,
    std::unordered_map<std::string, AttackAnimationData>& attackAnimations);

  // loads single animation into an animation collection
  void LoadSingleAnimation(std::string_view collection, std::string_view name, const AnimationInfo& animInfo);
  //! Registers new animation or gets existing if name exists in table
  unsigned int RegisterNewCollection(const std::string& lookUpString);

  //! for now just max out at 10
  std::array<AnimationCollection, 10> _collections;
  //! existing collections in archive
  unsigned int _livingCollectionCount;

  //! maps name strings to index in array
  std::unordered_map<std::string, unsigned int> _idLookupTable;

};