#pragma once

#include <Engine/Window.hpp>
#include <Engine/World.hpp>
#include <Engine/Render/Resources.hpp>
#include <Engine/Render/Renderer.hpp>
#include <Engine/Physics/Physics.hpp>

struct EngineComponents
{
    Window* window;
    World* world;
    Resources* resources;
    Renderer* renderer;
    Physics* physics;
};