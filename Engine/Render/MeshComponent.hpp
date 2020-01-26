#pragma once

#include <d3d11.h>

#include <Engine/Object.hpp>
#include <ThirdParty/glm/gtx/compatibility.hpp>

class Mesh : public Component
{
public:
    Mesh()
        : Component(Component::Type::Renderable)
    {}

    ID3D11Buffer* vert_vb;
    ID3D11Buffer* norm_vb;
    ID3D11Buffer* tcoords_vb;
    ID3D11Buffer* indexBuffer = nullptr;

    //Separate to material
    ID3D11ShaderResourceView* albedo = nullptr;
    ID3D11ShaderResourceView* normal = nullptr;
    ID3D11ShaderResourceView* metalness = nullptr;
    ID3D11ShaderResourceView* rough = nullptr;
    int numIndices = 0;
    std::string name;
    glm::mat4 worldMatrix = glm::mat4(1.0f);
    glm::vec3 minCoord = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 maxCoord = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);
};