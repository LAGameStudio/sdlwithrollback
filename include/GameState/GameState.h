#pragma once
#include "Input.h"
#include <memory>
#include "Player.h"
#include "Actions.h"

class Entity;
class Camera;

template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class ByteBuffer : public std::basic_streambuf<CharT, TraitsT>
{
public:
  ByteBuffer(std::vector<CharT>& vec)
  {
    setg(vec.data(), vec.data(), vec.data() + vec.size());
  }
};

class EntityCreation
{
public:
  static LocalPlayer CreateLocalPlayer(IInputHandler<SDL_Event>* input, float xOffset);
  #ifdef _WIN32
  static NetworkPlayer CreateNetworkPlayer(IInputHandler<GGPOInput>* input, float xOffset);
  #endif
  static std::shared_ptr<Camera> CreateCamera();
};

//______________________________________________________________________________
//!
class IGameState
{
public:
  virtual ~IGameState() {}
  //virtual void LoadAssets() = 0;
  virtual void ProcessRawInputs(SDL_Event* localInput) = 0;
  //gets active context for the player
  virtual GameContext GetActiveContext(const LocalPlayer& player) = 0;

  virtual void SaveGameState(ByteBuffer<char>& data) = 0;
  virtual void LoadGameState(ByteBuffer<char>& data) = 0;

  virtual Camera* GetCamera() = 0;
};

class MatchBase : public IGameState
{
public:
  MatchBase() : _mainCamera(EntityCreation::CreateCamera()) {}
  virtual Camera* GetCamera() override { return _mainCamera.get(); }

protected:
  //! Camera used by rendering pipeline to view the scene
  std::shared_ptr<Camera> _mainCamera;
  
};

class LocalMatch : public MatchBase
{
public:
  LocalMatch(IInputHandler<SDL_Event>* p1, IInputHandler<SDL_Event>* p2) :
    _player1(EntityCreation::CreateLocalPlayer(p1, 0)),
    _player2(EntityCreation::CreateLocalPlayer(p2, 150)),
    MatchBase()
  {}

  virtual void ProcessRawInputs(SDL_Event* localInput) override;

  virtual GameContext GetActiveContext(const LocalPlayer& player) override;

  virtual void SaveGameState(ByteBuffer<char>& data) override
  {
    std::ostream os(&data);
    os << _player1 << _player2;
  }

  virtual void LoadGameState(ByteBuffer<char>& data) override
  {
    std::istream is(&data);
    is >> _player1;
    is >> _player2;
  }
  
protected:
  //! 
  LocalPlayer _player1, _player2;

};