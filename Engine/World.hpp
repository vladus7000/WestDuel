#pragma once

#include <string>
#include <vector>
#include <map>

#include <d3d11.h>

#include <Engine/Camera.hpp>
#include <Engine/Render/Texture.hpp>
#include <Engine/Render/Light.hpp>
#include <Engine/Object.hpp>
#include <Engine/Physics/Physics.hpp>
#include <Engine/Physics/PhysicsComponent.hpp>
#include <Engine/Render/MeshComponent.hpp>

#include <ThirdParty/glm/gtx/compatibility.hpp>
#include <ThirdParty/tiny_obj_loader.h>
#include <Engine/ThirdParty/assimp/Importer.hpp>
#include <Engine/ThirdParty/assimp/scene.h>
#include <Engine/ThirdParty/assimp/postprocess.h>

class Resources;

class World
{
public:
	World(Physics* physics);
	~World();

	void initSun(Resources& resources);

	std::vector<std::shared_ptr<Mesh>>::iterator loadObjects(const std::string& fileName, const std::string& materialBaseDir, Resources& resources);

    void loadScene(const std::string& fileName, Resources& res);

	World(const World& rhs) = delete;
	World(World&& rhs) = delete;
	World& operator=(const World& rhs) = delete;
	World& operator=(World&& rhs) = delete;

	void setCamera(Camera cam) { m_camera = cam; }
	Camera& getCamera() { return m_camera; }

	const std::vector<std::shared_ptr<Mesh>>& getDrawable() const { return m_drawable; }
	std::vector<std::shared_ptr<Mesh>>& getDrawable() { return m_drawable; }

	void addLight(Light l);
	std::vector<Light>& getLights() { return m_lights; }
	const std::vector<Light>& getLights() const { return m_lights; }

	bool getIsDay() { return m_isDay; }

	void setSunAngle(float angle) { m_sunAngle = angle; }
	float getSunAngle() { return m_sunAngle; }

	void updateSun(float dt);

private:
	void initializeBuffers(Resources& resources);
	void deinitializeBuffers();

    void processNode(std::shared_ptr<Object> root, aiNode * node, const aiScene * scene, Resources& resources);
    std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene, Resources& resources);
    std::shared_ptr<PhysicsComponent> processPhysics(aiMesh* mesh, const aiScene* scene, Resources& resources);

private:
	Camera m_camera;
	tinyobj::attrib_t m_attrib;
	std::vector<tinyobj::shape_t> m_shapes;
	std::vector<tinyobj::material_t> m_materials;
	std::vector<std::shared_ptr<Mesh>> m_drawable;
	std::vector<Light> m_lights;
    std::shared_ptr<Object> m_root = std::make_shared<Object>();

	std::map<std::string, Texture> m_textures;
	std::string m_materialBaseDir;

    std::shared_ptr<Mesh> m_sunObject;
	Light* m_sunLight;
	bool m_isDay = true;
	float m_sunAngle = 0.0f;
	glm::vec3 m_sunColorMorning;
	glm::vec3 m_sunColorDay;

    Physics* m_physics;
};