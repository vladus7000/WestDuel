#pragma once

#include <Engine/Render/Pass.hpp>
#include <Engine/Render/Shader.hpp>
#include <Engine/Render/Texture.hpp>

struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11DepthStencilState;
class SkyAndFog : public Pass
{
public:
	SkyAndFog() {}
private:
	~SkyAndFog();
	virtual void setup(Renderer& renderer, Resources& resources) override;
	virtual void release(Renderer& renderer, Resources& resources) override;
	virtual void execute(Renderer& renderer) override;

private:
	ID3D11Buffer* m_constantBuffer = nullptr;
	ID3D11Buffer* m_skyBox = nullptr;
	ID3D11DepthStencilState* m_depthState;
	Shader m_mainShader;
	Texture* m_nightCubeMap;
	Texture* m_dayCubeMap;
};
