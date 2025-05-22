#pragma once

#include "DepthStencilStateBase.h"
#include "PipelineStruct.h"

class DepthStencilStateFactory
{
public:

	static D3D12_DEPTH_STENCIL_DESC& GetDepthStencilState(PipelineType type);

};

// ============================================================
// Object3d
// ============================================================

class Object3dDepthStencilState : public DepthStencilStateBase
{
public:

	D3D12_DEPTH_STENCIL_DESC& BuildDepthStencil() override
	{
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		return depthStencilDesc;
	}
};

// ============================================================
// Sprite
// ============================================================

class SpriteDepthStencilState : public DepthStencilStateBase
{
public:

	D3D12_DEPTH_STENCIL_DESC& BuildDepthStencil() override
	{
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		return depthStencilDesc;
	}
};

// ============================================================
// Line3d
// ============================================================

class Line3dDepthStencilState : public DepthStencilStateBase
{
public:

	D3D12_DEPTH_STENCIL_DESC& BuildDepthStencil() override
	{
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		return depthStencilDesc;
	}
};

// ============================================================
// Particle
// ============================================================

class ParticleDepthStencilState : public DepthStencilStateBase
{
public:

	D3D12_DEPTH_STENCIL_DESC& BuildDepthStencil() override
	{
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		return depthStencilDesc;
	}
};

// ============================================================
// PrimitiveDrawr
// ============================================================

class PrimitiveDrawrDepthStencilState : public DepthStencilStateBase
{
public:

	D3D12_DEPTH_STENCIL_DESC& BuildDepthStencil() override
	{
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		return depthStencilDesc;
	}
};

// ============================================================
// RenderTexture
// ============================================================

class RenderTextureDepthStencilState : public DepthStencilStateBase
{
public:

	D3D12_DEPTH_STENCIL_DESC& BuildDepthStencil() override
	{
		depthStencilDesc.DepthEnable = false;
		return depthStencilDesc;
	}
};

// ============================================================
// Animation
// ============================================================

class AnimationDepthStencilState : public DepthStencilStateBase
{
public:

	D3D12_DEPTH_STENCIL_DESC& BuildDepthStencil() override
	{
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		return depthStencilDesc;
	}
};

// ============================================================
// Skybox
// ============================================================

class SkyboxDepthStencilState : public DepthStencilStateBase
{
public:

	D3D12_DEPTH_STENCIL_DESC& BuildDepthStencil() override
	{
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		return depthStencilDesc;
	}
};