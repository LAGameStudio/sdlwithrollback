// 2DEngine.cpp : Defines the entry point for the console application.
//

#include "../include/GameManagement.h"


int main()
{
  GameManager::Get().Initialize();
  GameManager::Get().BeginGameLoop();
  return 0;
}

