#pragma once

#include "RasterizerStateBase.h"
#include "PipelineStruct.h"

class RasterizerStateFactory
{
public:

	static D3D12_RASTERIZER_DESC& GetRasterizerDesc(PipelineType type);

};

// ============================================================
// Object3d
// ============================================================

class Object3dRasterizerState : public RasterizerStateBase
{
public:

	D3D12_RASTERIZER_DESC& BuildRasterizer() override
	{
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		return rasterizerDesc;
	}
};

// ============================================================
// Sprite
// ============================================================

class SpriteRasterizerState : public RasterizerStateBase
{
public:

	D3D12_RASTERIZER_DESC& BuildRasterizer() override
	{
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		return rasterizerDesc;
	}
};

// ============================================================
// Line3d
// ============================================================

class Line3dRasterizerState : public RasterizerStateBase
{
public:

	D3D12_RASTERIZER_DESC& BuildRasterizer() override
	{
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.DepthClipEnable = TRUE;
		return rasterizerDesc;
	}
};

// ============================================================
// Particle
// ============================================================

class ParticleRasterizerState : public RasterizerStateBase
{
public:

	D3D12_RASTERIZER_DESC& BuildRasterizer() override
	{
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		return rasterizerDesc;
	}
};

// ============================================================
// PrimitiveDrawr
// ============================================================

class PrimitiveDrawrRasterizerState : public RasterizerStateBase
{
public:

	D3D12_RASTERIZER_DESC& BuildRasterizer() override
	{
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		return rasterizerDesc;
	}
};

// ============================================================
// RenderTexture
// ============================================================

class RenderTextureRasterizerState : public RasterizerStateBase
{
public:

	D3D12_RASTERIZER_DESC& BuildRasterizer() override
	{
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		return rasterizerDesc;
	}
};

// ============================================================
// Animation
// ============================================================

class AnimationRasterizerState : public RasterizerStateBase
{
public:

	D3D12_RASTERIZER_DESC& BuildRasterizer() override
	{
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		return rasterizerDesc;
	}
};

// ============================================================
// Skybox
// ============================================================

class SkyboxRasterizerState : public RasterizerStateBase
{
public:

	D3D12_RASTERIZER_DESC& BuildRasterizer() override
	{
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		return rasterizerDesc;
	}
};