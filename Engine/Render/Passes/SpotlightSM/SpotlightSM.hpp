#pragma once

#include <Engine/Render/Pass.hpp>
#include <Engine/Render/Shader.hpp>
#include <Engine/Render/Texture.hpp>

class SpotlightSM : public Pass
{
public:
	SpotlightSM() {}
private:
	~SpotlightSM();
	virtual void setup(Renderer& renderer, Resources& resources) override;
	virtual void release(Renderer& renderer, Resources& resources) override;
	virtual void execute(Renderer& renderer) override;

private:
	Shader m_mainShader;
};
