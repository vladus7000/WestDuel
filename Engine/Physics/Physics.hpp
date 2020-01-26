#pragma once

#include <vector>
#include <Engine/Physics/PhysicsComponent.hpp>

#include <Engine\ThirdParty\Bullet\btBulletDynamicsCommon.h>

class Physics
{
public:
    Physics();
    ~Physics();

    Physics(const Physics& rhs) = delete;
    Physics(Physics&& rhs) = delete;
    Physics& operator=(const Physics& rhs) = delete;
    Physics& operator=(Physics&& rhs) = delete;

    void addPhysicsBody(std::shared_ptr<PhysicsComponent> component);
    void step();
    void syncPhysicsTransformToRenderables();

private:
    btDiscreteDynamicsWorld* m_dynamicsWorld;
    btSequentialImpulseConstraintSolver* m_solver;
    btBroadphaseInterface* m_overlappingPairCache;
    btCollisionDispatcher* m_dispatcher;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    std::vector<btCollisionShape*> m_collisionShapes;
};