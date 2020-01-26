#include <Engine\Physics\Physics.hpp>
#include <Engine\Render\MeshComponent.hpp>
#include <ThirdParty/glm/gtc/matrix_transform.hpp>
#include <ThirdParty/glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

Physics::Physics()
{
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_overlappingPairCache = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver;

    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

    m_dynamicsWorld->setGravity(btVector3(0, -1, 0));
}

Physics::~Physics()
{
    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    for (auto shape : m_collisionShapes)
    {
        delete shape;
    }

    delete m_dynamicsWorld;
    delete m_solver;
    delete m_overlappingPairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

void Physics::addPhysicsBody(std::shared_ptr<PhysicsComponent> component)
{
    btCollisionShape* colShape = nullptr;
    if (component->bb_type == 0)
    {
        colShape = new btSphereShape(btScalar(1.));
    }
    else
    {
        glm::vec3 half = (component->maxCoord - component->minCoord) / 2.0f;
        colShape = new btBoxShape(btVector3(half.x, half.y, half.z));
    }
    m_collisionShapes.push_back(colShape);


    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(component->mass);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        colShape->calculateLocalInertia(mass, localInertia);

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(component->worldMatrix, scale, rotation, translation, skew, perspective);

    startTransform.setOrigin(btVector3(translation.x, translation.y, translation.z));

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    body->setUserPointer(component.get());

    m_dynamicsWorld->addRigidBody(body);
}

void Physics::step()
{
    m_dynamicsWorld->stepSimulation(1.f / 60.f, 10);
}

void Physics::syncPhysicsTransformToRenderables()
{
    for (int j = m_dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[j];
        btRigidBody* body = btRigidBody::upcast(obj);
        btTransform trans;
        if (body && body->getMotionState())
        {
            body->getMotionState()->getWorldTransform(trans);
        }
        else
        {
            trans = obj->getWorldTransform();
        }

        if (Component* component = (Component*)body->getUserPointer())
        {
            auto& meshes = component->getOwner()->getComponents(Component::Type::Renderable);
            for (auto& meshComponent : meshes)
            {
                Mesh* r = (Mesh*)meshComponent.get();
                r->worldMatrix = glm::translate(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
            }
        }
    }
}
