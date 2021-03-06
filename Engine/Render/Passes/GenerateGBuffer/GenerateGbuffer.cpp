#include <Engine/Render/Passes/GenerateGBuffer/GenerateGbuffer.hpp>
#include <Engine/Render/Renderer.hpp>
#include <Engine/Render/GBuffer.hpp>
#include <Engine/Render/Resources.hpp>
#include <Engine/Window.hpp>
#include <Engine/World.hpp>


GenerateGBuffer::~GenerateGBuffer()
{
	if (m_constantBuffer)
	{
		m_constantBuffer->Release();
	}
}

void GenerateGBuffer::setup(Renderer& renderer, Resources& resources)
{
	if (!m_constantBuffer)
	{
		auto device = resources.getDevice();
		D3D11_BUFFER_DESC buffDesc;
		buffDesc.ByteWidth = sizeof(float[16]) * 2; // must be multiply of 16
		buffDesc.Usage = D3D11_USAGE_DYNAMIC;
		buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffDesc.MiscFlags = 0;
		device->CreateBuffer(&buffDesc, nullptr, &m_constantBuffer);

		m_mainShader = resources.createShader("shaders/generateGBuffer/generateGBuffer.hlsl", "vsmain", "psmain");
		m_sampler = resources.getResource<ID3D11SamplerState>(Resources::ResoucesID::LinearSampler);
		m_depthState = resources.getResource<ID3D11DepthStencilState>(Resources::ResoucesID::EqualDepthState);
	}
}

void GenerateGBuffer::release(Renderer& renderer, Resources& resources)
{
	auto context = renderer.getContext();
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	context->IASetInputLayout(nullptr);
	ID3D11Buffer* buffers[] = { nullptr};
	context->IASetVertexBuffers(0, 0, buffers, nullptr, nullptr);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED);

	ID3D11Buffer* constants[] = { nullptr };
	context->PSSetConstantBuffers(0, 1, constants);
	context->VSSetConstantBuffers(0, 1, constants);

	ID3D11SamplerState* samplers[] = { nullptr };
	context->PSSetSamplers(0, 1, samplers);

	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr,nullptr,nullptr };
	context->PSSetShaderResources(0, 4, srvs);

	ID3D11RenderTargetView* rtvs[] = { nullptr , nullptr , nullptr };;
	context->OMSetRenderTargets(3, rtvs, nullptr);
	context->OMSetDepthStencilState(nullptr, 0);
}

void GenerateGBuffer::execute(Renderer& renderer)
{
	auto context = renderer.getContext();
	auto& world = renderer.getWorld();

	auto& gbuffer = renderer.getGBuffer();
	gbuffer.bindForWriting(renderer);
	gbuffer.clearColor(renderer);

	context->VSSetShader(m_mainShader.getVertexShader(), nullptr, 0);
	context->PSSetShader(m_mainShader.getPixelShader(), nullptr, 0);
	context->IASetInputLayout(m_mainShader.getInputLayout());
	
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->OMSetDepthStencilState(m_depthState, 0);

	ID3D11Buffer* constants[] = { m_constantBuffer };
	context->PSSetConstantBuffers(0, 1, constants);
	context->VSSetConstantBuffers(0, 1, constants);
	ID3D11SamplerState* samplers[] = { m_sampler };
	context->PSSetSamplers(0, 1, samplers);

	for (auto& mesh : world.getDrawable())
	{
		D3D11_MAPPED_SUBRESOURCE res;
		context->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
		struct Data
		{
			float mvp[16];
			float model[16];
		};
		Data* buffer = reinterpret_cast<Data*>(res.pData);

		auto& camera = renderer.getWorld().getCamera();

		glm::mat4 mvp = camera.getProjection() * camera.getView() * mesh->worldMatrix;
	
		//glm::mat4 model = glm::rotate(glm::radians(0.0f), glm::vec3{ 0.f, 1.f, 0.f }) * glm::scale(glm::vec3{ 1.0f, 1.0f, 1.0f });
		memcpy(buffer->mvp, &mvp[0][0], sizeof(float[16]));
		memcpy(buffer->model, &(mesh->worldMatrix[0][0]), sizeof(float[16]));
		context->Unmap(m_constantBuffer, 0);

		ID3D11ShaderResourceView* srvs[] = { mesh->albedo, mesh->normal, mesh->metalness, mesh->rough};
		context->PSSetShaderResources(0, 4, srvs);

		unsigned int stride[] = { sizeof(float[3]), sizeof(float[3]) , sizeof(float[2]) };
		unsigned int offsets[] = { 0, 0, 0 };
		ID3D11Buffer* buffers[] = { mesh->vert_vb, mesh->norm_vb, mesh->tcoords_vb };
		context->IASetVertexBuffers(0, 3, buffers, stride, offsets);
        if (mesh->indexBuffer)
        {
            context->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

            context->DrawIndexed(mesh->numIndices, 0, 0);
        }
        else
        {
            context->Draw(mesh->numIndices, 0);
        }

		//context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//context->DrawIndexed(mesh.numIndices, 0, 0);
	}
}

