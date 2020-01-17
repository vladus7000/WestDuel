#pragma once

#include <Engine/Engine.hpp>

#define CREATE_GAME_DECL Application* CreateGame();
#define CREATE_GAME CreateGame
#define CREATE_GAME_IMPL(name) Application* CreateGame(){return new name;}

class Application
{
public:
    Application() = default;
    virtual ~Application() {}

public:
    virtual void engineStared(EngineComponents engine) = 0;
    virtual void exitRequested() = 0;
    virtual bool shouldExit() = 0;
    virtual void engineStopped() = 0;

    virtual void logicalUpdate(float dt) = 0;
    virtual void uiUpdate(float dt) = 0;
};