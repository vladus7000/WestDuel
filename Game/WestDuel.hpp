#pragma once

#include <Engine/Application.hpp>

class WestDuel : public Application
{
public:
    WestDuel();
    ~WestDuel();

protected:
    virtual void engineStared(EngineComponents engineComponents) override;
    virtual void exitRequested() override;
    virtual bool shouldExit() override;
    virtual void engineStopped() override;

    virtual void logicalUpdate(float dt) override;
    virtual void uiUpdate(float dt) override;

private:
    int m_zombieCount = 5;
    int m_lightNumber = 50;
    EngineComponents m_engine;
    std::vector<glm::vec3> targetPositions;
    glm::vec3 minBB = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 maxBB = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);

    std::vector<World::Mesh>::iterator pistol;
    std::vector<World::Mesh>::iterator zombie;
};