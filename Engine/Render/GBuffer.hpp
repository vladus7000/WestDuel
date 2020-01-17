#pragma once

#include <Engine/Render/Texture.hpp>

class Renderer;
class Resources;

class GBuffer
{
public:
	GBuffer(Renderer& renderer, Resources& resources);
	~GBuffer();

	void bindForWriting(Renderer& renderer);
	void clearColor(Renderer& renderer);
	void clearDepth(Renderer& renderer);

	Texture m_diffuse;
	Texture m_normal_metalnes;
	Texture m_position_rough;
	Texture m_depth;
};