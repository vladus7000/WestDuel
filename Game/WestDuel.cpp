#include <algorithm>
#include <random>
#include <vector>

#include <Game/WestDuel.hpp>

#include <Engine/Camera.hpp>
#include <Engine/SettingsHolder.hpp>
#include <Engine/Settings/RenderSettings.hpp>
#include <Engine/Settings/WorldSettings.hpp>
#include <Engine/UI/UI.hpp>

CREATE_GAME_IMPL(WestDuel);

Camera g_mainCamera;

WestDuel::WestDuel()
{
}

WestDuel::~WestDuel()
{
}

void WestDuel::engineStared(EngineComponents engine)
{
    m_engine = engine;
    auto& resources = *engine.resources;
    auto& mainWorld = *engine.world;
    auto& mainWindow = *engine.window;

    m_engine.window->setStickCursorToCenter(true);
    m_engine.window->setCursorVisibility(false);
    //engine.window->setUserFunction(UserFunc);

    engine.window->addKeysListener(std::bind(&WestDuel::keyUpdate, this, std::placeholders::_1));
    engine.window->addMouseListener(std::bind(&WestDuel::mouseUpdate, this, std::placeholders::_1, std::placeholders::_2));

    mainWorld.loadScene("Data/scene/scene.fbx", resources);

    auto newObjects = mainWorld.loadObjects("rungholt/house.obj", "rungholt/", resources);
    while (newObjects != mainWorld.getObjects().end())
    {
        //newObjects->worldMatrix = glm::scale(glm::vec3{ 0.1f, 0.1f, 0.1f });
#undef min
#undef max
        minBB.x = std::min(minBB.x, newObjects->minCoord.x);
        minBB.y = std::min(minBB.y, newObjects->minCoord.y);
        minBB.z = std::min(minBB.z, newObjects->minCoord.z);

        maxBB.x = std::max(maxBB.x, newObjects->maxCoord.x);
        maxBB.y = std::max(maxBB.y, newObjects->maxCoord.y);
        maxBB.z = std::max(maxBB.z, newObjects->maxCoord.z);

        newObjects->worldMatrix = glm::scale(glm::vec3{ 0.2f, 0.2f, 0.2f });
        ++newObjects;
    }
    minBB -= 50.0f;
    maxBB += 50.0f;

    //auto groundObject = mainWorld.loadObjects("cube.obj", "", resources);
    //groundObject->worldMatrix = glm::scale(glm::vec3{ 800.0f, 0.1f, 800.0f });
    //groundObject->name = "ground";

    pistol = mainWorld.loadObjects("pistol/pistol.obj", "pistol/", resources);

    auto t = pistol;
    while (t != mainWorld.getObjects().end())
    {
        t->name = "pistol";
        t->worldMatrix = glm::translate(glm::vec3{ 0.0f, 5.0f, -40.0f }) * glm::scale(glm::vec3{ 15.0f, 15.0f, 15.0f });
        ++t;
    }

    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> zombieR(-400.0f, 400.0f);
    std::uniform_real_distribution<> distX(minBB.x, maxBB.x);
    std::uniform_real_distribution<> distY(minBB.y, maxBB.y);
    std::uniform_real_distribution<> distZ(minBB.z, maxBB.z);
    std::uniform_real_distribution<> colorRGB(15.0f, 30.0f);
    std::uniform_real_distribution<> radiuses(0.1f, 15.0f);

    Light l;
    for (int i = 0; i < m_lightNumber; i++)
    {
        l.m_position = { distX(e2), distY(e2), distZ(e2) };
        targetPositions.push_back(l.m_position);
        l.m_intensity = { colorRGB(e2), colorRGB(e2), colorRGB(e2) };
        //float in = glm::dot({ 0.2126f, 0.7152f, 0.0722f }, l.m_intensity);
        l.m_type = Light::Type::Point;
        l.m_radius = radiuses(e2);
        mainWorld.addLight(l);
    }
    zombie = mainWorld.loadObjects("zombie/0.obj", "zombie/", resources); // 5 shapes each
    for (int i = 0; i < m_zombieCount - 1; i++)
        mainWorld.loadObjects("zombie/0.obj", "zombie/", resources); // 5 shapes each

    {
        auto t_ = zombie;
        float rx, ry;
        for (int i = 0; i < m_zombieCount; i++)
        {
            rx = zombieR(e2);
            ry = zombieR(e2);
            for (int j = 0; j < 5; j++)
            {
                t_->worldMatrix = glm::translate(glm::vec3{ rx, 0.0f, ry }) * glm::scale(glm::vec3{ 6.0f, 6.0f, 6.0f });
                ++t_;
            }
        }
    }
    g_mainCamera.setProjection(60.0f, (float)mainWindow.getWidth() / (float)mainWindow.getHeight(), 0.01f, 1000.f);
    g_mainCamera.setView({ 120, 60, 4 }, { 0, 0, 0 });

}

void WestDuel::exitRequested()
{
}

bool WestDuel::shouldExit()
{
    return true;
}

void WestDuel::engineStopped()
{
}

void WestDuel::logicalUpdate(float dt)
{
    const float speed = dt * 50.0f;
    float delta = 9.0f * dt;
    auto& lights = m_engine.world->getLights();

    std::random_device rd;
    std::mt19937 e2(rd());

    for (int i = 0; i < m_lightNumber; i++)
    {
        if (glm::length(lights[i + 1].m_position - targetPositions[i]) < 0.5f)
        {
            std::uniform_real_distribution<> distX(minBB.x, maxBB.x);
            std::uniform_real_distribution<> distY(minBB.y, maxBB.y);
            std::uniform_real_distribution<> distZ(minBB.z, maxBB.z);


            targetPositions[i] = { distX(e2), distY(e2), distZ(e2) };
        }
        lights[i + 1].m_position += glm::normalize(targetPositions[i] - lights[i + 1].m_position) * delta;
    }

    g_mainCamera.getPosition().y = 7.0f;
    g_mainCamera.updateView();
    m_engine.world->setCamera(g_mainCamera);

    auto t = pistol;
    for (int i = 0; i < 4; i++)
    {
        t->worldMatrix = glm::inverse(g_mainCamera.getView()) *glm::translate(glm::vec3(0.4f, -0.5f, 1.03f))*glm::rotate(glm::radians(90.0f), glm::vec3{ 0.0f, 1.0f, 0.0f })* glm::scale(glm::vec3{ 0.6f, 0.6f, 0.6f });// glm::translate(glm::vec3{ 0.0f, 5.0f, -40.0f }) *glm::rotate(glm::radians(angle), glm::vec3{ 0.0f, 1.0f, 0.0f }) * glm::scale(glm::vec3{ 15.0f, 15.0f, 15.0f });
        ++t;
    }

    {
        std::uniform_real_distribution<> zombieR(-400.0f, 400.0f);
        auto t_ = zombie;
        float rx, ry;
        for (int i = 0; i < m_zombieCount; i++)
        {
            rx = zombieR(e2);
            ry = zombieR(e2);
            for (int j = 0; j < 5; j++)
            {
                glm::vec4 pos = t_->worldMatrix[3];

                if (glm::length(glm::vec3(pos.x, pos.y, pos.z) - g_mainCamera.getPosition()) > 0.5f)
                {
                    glm::vec3 newPos = glm::normalize(g_mainCamera.getPosition() - glm::vec3(pos.x, pos.y, pos.z)) * delta;
                    pos.x += newPos.x;
                    pos.z += newPos.z;
                }

                t_->worldMatrix[3] = pos;
                ++t_;
            }
        }
    }
}

void WestDuel::uiUpdate(float dt)
{
}

void WestDuel::keyUpdate(const std::array<Window::KeyState, 256>& state)
{
    bool wPressed = state[(int)Window::Keys::W] == Window::KeyState::Down;
    bool aPressed = state[(int)Window::Keys::A] == Window::KeyState::Down;
    bool sPressed = state[(int)Window::Keys::S] == Window::KeyState::Down;
    bool dPressed = state[(int)Window::Keys::D] == Window::KeyState::Down;
    bool escPressed = state[(int)Window::Keys::Esc] == Window::KeyState::FirstDown;

    if (escPressed)
    {
        m_inGame = !m_inGame;

        if (!m_inGame)
        {
            m_engine.window->setStickCursorToCenter(false);
            m_engine.window->setCursorVisibility(true);
        }
        else
        {
            m_engine.window->setStickCursorToCenter(true);
            m_engine.window->setCursorVisibility(false);
        }
    }

    float speed = 50.0f * 1.0f / 60.0f;
    if (wPressed)
    {
        g_mainCamera.moveForward(speed);
    }
    if (sPressed)
    {
        g_mainCamera.moveBackward(speed);
    }

    if (aPressed)
    {
        g_mainCamera.moveLeft(speed);
    }

    if (dPressed)
    {
        g_mainCamera.moveRight(speed);
    }

}

void WestDuel::mouseUpdate(const std::array<Window::KeyState, 3>& mouseKeys, const Window::MouseMove& mouseMove)
{
    const float dt = 1.0f / 60.0f;
    if (m_inGame)
    {
        static float yaw = 0.0f;
        static float pitch = 0.0f;

        if (mouseMove.dx != 0.0f || mouseMove.dy != 0.0f)
        {
            yaw += -mouseMove.dx;
            pitch += -mouseMove.dy;

            g_mainCamera.rotate(pitch, yaw);
        }
    }
}
