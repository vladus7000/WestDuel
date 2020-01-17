#include <Engine/Render/Passes/MainZpass/MainZpass.hpp>
#include <Engine/Render/Renderer.hpp>
#include <Engine/Render/GBuffer.hpp>
#include <Engine/Render/Resources.hpp>
#include <Engine/Window.hpp>
#include <Engine/World.hpp>

MainZpass::~MainZpass()
{
}

void MainZpass::setup(Renderer& renderer, Resources& resources)
{
}

void MainZpass::release(Renderer& renderer, Resources& resources)
{
}

void MainZpass::execute(Renderer& renderer)
{
	auto context = renderer.getContext();
	auto& world = renderer.getWorld();
	auto& gbuffer = renderer.getGBuffer();
	gbuffer.clearDepth(renderer);

	auto& camera = renderer.getWorld().getCamera();
	renderer.depthPrepass(camera, gbuffer.m_depth, 0.0f, 0.0f, gbuffer.m_depth.m_w, gbuffer.m_depth.m_h);
}
