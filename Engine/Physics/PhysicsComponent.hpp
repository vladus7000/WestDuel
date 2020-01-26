#pragma once

#include <Engine/Object.hpp>
#include <Engine/Physics/Physics.hpp>

#include <ThirdParty/glm/gtx/compatibility.hpp>

class PhysicsComponent : public Component
{
public:
    PhysicsComponent()
        : Component(Component::Type::Physics)
    {}

    glm::mat4 worldMatrix = glm::mat4(1.0f);
    glm::vec3 minCoord = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 maxCoord = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);
    float mass = 0.0f;
    int bb_type = 0; // 0 - sphere, 1 - box
};