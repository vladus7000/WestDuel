#include <fstream>
#include <streambuf>
#include <iostream>

#include <D3DX11tex.h>

#include <Engine/Render/Resources.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <Engine/ThirdParty/stb_image.h>


Resources::Resources()
{
	D3D_FEATURE_LEVEL level[] = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL outLevel;
	unsigned int flags = D3D11_CREATE_DEVICE_DEBUG;
	D3D11CreateDevice(m_adapter, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, level, 1, D3D11_SDK_VERSION, &m_device, &outLevel, &m_context);
	createDefaultResources();
}

Resources::~Resources()
{
	destroyDefaultResources();
	m_device->Release();
	m_context->Release();
}

void Resources::registerResource(ResoucesID id, void* resource)
{
	auto foundIt = m_textureResources.find(id);
	if (foundIt == m_textureResources.end())
	{
		m_textureResources[id] = resource;
	}
}

void Resources::unregisterResource(ResoucesID id)
{
	auto foundIt = m_textureResources.find(id);
	if (foundIt != m_textureResources.end())
	{
		m_textureResources.erase(foundIt);
	}
}

Texture Resources::loadTexture(const std::string& fileName, bool createSampler)
{
	Texture res;
	D3DX11CreateShaderResourceViewFromFile(m_device, (DataDir + fileName).c_str(), 0, 0, res.m_SRV.GetAddressOf(), nullptr);

	if (createSampler)
	{
		D3D11_SAMPLER_DESC sampler;
		ZeroMemory(&sampler, sizeof(D3D11_SAMPLER_DESC));
		sampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		m_device->CreateSamplerState(&sampler, res.m_sampler.GetAddressOf());
	}

	return res;
}

Texture Resources::loadHDRTexture(const std::string& fileName, bool createSampler)
{
	Texture res;

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf((DataDir + fileName).c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = data;

		m_device->CreateTexture2D(&desc, nullptr, res.m_texture.GetAddressOf());
		free(data);

		m_device->CreateShaderResourceView(res.m_texture.Get(), nullptr, res.m_SRV.GetAddressOf());

		if (createSampler)
		{
			D3D11_SAMPLER_DESC sampler;
			ZeroMemory(&sampler, sizeof(D3D11_SAMPLER_DESC));
			sampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			m_device->CreateSamplerState(&sampler, res.m_sampler.GetAddressOf());
		}
	}

	return res;
}

Texture Resources::createDepthStencilTexture(int w, int h)
{
	Texture ret;

	ret.m_w = w;
	ret.m_h = h;
	{ //depth stencil
		D3D11_TEXTURE2D_DESC desc;
		desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;//DXGI_FORMAT_D32_FLOAT_S8X24_UINT
		desc.Width = w;
		desc.Height = h;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		m_device->CreateTexture2D(&desc, nullptr, ret.m_texture.GetAddressOf());

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		m_device->CreateDepthStencilView(ret.m_texture.Get(), &dsvDesc, ret.m_DSV.GetAddressOf());
		m_device->CreateShaderResourceView(ret.m_texture.Get(), &srvDesc, ret.m_SRV.GetAddressOf());
	}

	return ret;
}

void Resources::createDefaultResources()
{
	D3D11_SAMPLER_DESC sampler;
	ZeroMemory(&sampler, sizeof(D3D11_SAMPLER_DESC));
	sampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	m_device->CreateSamplerState(&sampler, &m_linearSampler);
	registerResource(ResoucesID::LinearSampler, m_linearSampler);

	{
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		// Depth test parameters
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_EQUAL;

		// Stencil test parameters
		dsDesc.StencilEnable = false;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		m_device->CreateDepthStencilState(&dsDesc, &m_equalDepthState);
		registerResource(ResoucesID::EqualDepthState, m_equalDepthState);
	}
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		// Depth test parameters
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;

		// Stencil test parameters
		dsDesc.StencilEnable = false;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		m_device->CreateDepthStencilState(&dsDesc, &m_notEqualDepthState);
		registerResource(ResoucesID::NotEqualDepthState, m_notEqualDepthState);
	}
	{
		D3D11_BLEND_DESC blend;
		ZeroMemory(&blend, sizeof(D3D11_BLEND_DESC));
		blend.AlphaToCoverageEnable = false;
		blend.IndependentBlendEnable = false;
		blend.RenderTarget[0].BlendEnable = true;
		blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

		blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		m_device->CreateBlendState(&blend, &m_defaultAdditiveBlendState);
		registerResource(ResoucesID::DefaultAdditiveBlend, m_defaultAdditiveBlendState);
	}
}

void Resources::destroyDefaultResources()
{
	m_linearSampler->Release();
	m_equalDepthState->Release();
	m_notEqualDepthState->Release();
	m_defaultAdditiveBlendState->Release();
}

Shader Resources::createShader(const std::string& fileName, const std::string& vsName, const std::string& psName, std::vector<D3D11_INPUT_ELEMENT_DESC> layout)
{
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;

	ID3D10Blob* blob = nullptr;
	ID3D10Blob* blobErr = nullptr;

	if (!vsName.empty())
	{
		D3DX11CompileFromFile((DataDir+fileName).c_str(), NULL, NULL, vsName.c_str(), "vs_5_0", 0, 0, NULL, &blob, &blobErr, NULL);
		if (blobErr)
		{
			std::string err = (char*)blobErr->GetBufferPointer();
			OutputDebugStringA(err.c_str());
			blobErr->Release();
			blobErr = nullptr;
		}

		m_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader);

		D3D11_INPUT_ELEMENT_DESC desr[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		m_device->CreateInputLayout(layout.empty() ? desr : layout.data(), layout.empty() ? 3 : layout.size(), blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);

		blob->Release();
		blob = nullptr;
	}

	if (!psName.empty())
	{
		D3DX11CompileFromFile((DataDir + fileName).c_str(), NULL, NULL, psName.c_str(), "ps_5_0", 0, 0, NULL, &blob, &blobErr, NULL);
		if (blobErr)
		{
			std::string err = (char*)blobErr->GetBufferPointer();
			OutputDebugStringA(err.c_str());
			blobErr->Release();
		}

		m_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader);

		blob->Release();
		blob = nullptr;
	}

	Shader ret(inputLayout, vertexShader, pixelShader);
	if (inputLayout) inputLayout->Release();
	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();

	return ret;
}

Shader Resources::createComputeShader(const std::string& fileName, const std::string& csName)
{
	ID3D11ComputeShader* shader = nullptr;
	if (!csName.empty())
	{
		ID3D10Blob* blob = nullptr;
		ID3D10Blob* blobErr = nullptr;
		D3DX11CompileFromFile((DataDir + fileName).c_str(), NULL, NULL, csName.c_str(), "cs_5_0",0 , 0, NULL, &blob, &blobErr, NULL);
		if (blobErr)
		{
			std::string err = (char*)blobErr->GetBufferPointer();
			OutputDebugStringA(err.c_str());
			blobErr->Release();
			blobErr = nullptr;
		}

		if (blob)
		{
			m_device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);

			blob->Release();
			blob = nullptr;
		}
	}

	Shader res(shader);
	if (shader) shader->Release();
	return res;
}
