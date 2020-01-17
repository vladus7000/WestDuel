#include <Engine/Render/Passes/Tonemap/LuminanceAvg.hpp>
#include <Engine/Render/Renderer.hpp>
#include <Engine/Render/GBuffer.hpp>
#include <Engine/Render/Resources.hpp>
#include <Engine/Window.hpp>
#include <Engine/World.hpp>

LuminanceAvg::~LuminanceAvg()
{
}

void LuminanceAvg::setup(Renderer& renderer, Resources& resources)
{
	if (!m_mainShader.getPixelShader())
	{
		m_mainShader = resources.createShader("shaders/tonemap/tonemap.hlsl", "vsmain", "psmain");
		m_sampler = resources.getResource<ID3D11SamplerState>(Resources::ResoucesID::LinearSampler);
		m_depthState = resources.getResource<ID3D11DepthStencilState>(Resources::ResoucesID::NotEqualDepthState);
	}
}

void LuminanceAvg::release(Renderer& renderer, Resources& resources)
{
	ID3D11SamplerState* samplers[] = { nullptr };
	renderer.getContext()->PSSetSamplers(0, 1, samplers);
	renderer.getContext()->OMSetDepthStencilState(nullptr, 0);
}

void LuminanceAvg::execute(Renderer& renderer)
{
	auto context = renderer.getContext();
	auto& gbuffer = renderer.getGBuffer();
	{
		ID3D11RenderTargetView* rtvs[] = { renderer.getDisplayBB() };

		context->OMSetRenderTargets(1, rtvs, gbuffer.m_depth.m_DSV.Get());
		ID3D11ShaderResourceView* srvs[] = { renderer.getHDRTexture().m_SRV.Get() };
		context->PSSetShaderResources(0, 1, srvs);
	}
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(nullptr);
	context->OMSetDepthStencilState(m_depthState, 0);
	context->PSSetShader(m_mainShader.getPixelShader(), nullptr, 0);
	context->VSSetShader(m_mainShader.getVertexShader(), nullptr, 0);
	ID3D11SamplerState* samplers[] = { m_sampler };
	context->PSSetSamplers(0, 1, samplers);
	context->Draw(4, 0);
}
