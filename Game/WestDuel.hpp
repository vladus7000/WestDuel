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

    void keyUpdate(const std::array<Window::KeyState, 256>& state);
    void mouseUpdate(const std::array<Window::KeyState, 3>& mouseKeys, const Window::MouseMove& mouseMove);

private:
    int m_zombieCount = 1;
    int m_lightNumber = 10;
    bool m_inGame = true;
    EngineComponents m_engine;
    std::vector<glm::vec3> targetPositions;
    glm::vec3 minBB = glm::vec3(-100.0f, -100.0f, -100.0f);
    glm::vec3 maxBB = glm::vec3(100.0f, 100.0f, 100.0f);

    std::vector<std::shared_ptr<Mesh>>::iterator pistol;
    std::vector<std::shared_ptr<Mesh>>::iterator zombie;
};