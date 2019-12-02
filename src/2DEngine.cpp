// 2DEngine.cpp : Defines the entry point for the console application.
//
#include "GameManagement.h"

#ifdef _WIN32
#undef main
#endif

int main(int argc, char* args[])
{
  ResourceManager::Get().Initialize();
  GameManager::Get().Initialize();

  GameManager::Get().BeginGameLoop();

  return 0;
}
