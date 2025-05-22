#pragma once
#include <memory>
#include <string>
#include <cassert>

#include "CompileShaderBase.h"
#include "PipelineStruct.h"

class CompileShaderFactory
{
public:

	static D3D12_SHADER_BYTECODE& GetCompileShader_VS(
		PipelineType type,
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	);

	static D3D12_SHADER_BYTECODE& GetCompileShader_PS(
		PipelineType type,
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler,
		PostEffectType effectType = PostEffectType::None
	);

	static D3D12_SHADER_BYTECODE CreateCompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3> dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	);

private:

	static ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3> dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	);

	static std::unique_ptr<CompileShaderBase> GetCompileShaderPtr(
		PipelineType type,
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	);

};

// ============================================================
// Object3d
// ============================================================

class Object3dCompileShader : public CompileShaderBase
{
public:

	Object3dCompileShader(
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	) :CompileShaderBase(dxcUtils, dxcCompiler, includeHandler) {}

	D3D12_SHADER_BYTECODE& BuildVS_Shader() override
	{
		vsBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0", 
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return vsBytecode;
	}

	D3D12_SHADER_BYTECODE& BuildPS_Shader(PostEffectType type = PostEffectType::None) override
	{
		psBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return psBytecode;
	}
};

// ============================================================
// Sprite
// ============================================================

class SpriteCompileShader : public CompileShaderBase
{
public:

	SpriteCompileShader(
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	) :CompileShaderBase(dxcUtils, dxcCompiler, includeHandler) {}

	D3D12_SHADER_BYTECODE& BuildVS_Shader() override
	{
		vsBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Sprite.VS.hlsl", L"vs_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return vsBytecode;
	}

	D3D12_SHADER_BYTECODE& BuildPS_Shader(PostEffectType type = PostEffectType::None) override
	{
		psBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Sprite.PS.hlsl", L"ps_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return psBytecode;
	}
};

// ============================================================
// Line3d
// ============================================================

class Line3dCompileShader : public CompileShaderBase
{
public:

	Line3dCompileShader(
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	) :CompileShaderBase(dxcUtils, dxcCompiler, includeHandler) {}

	D3D12_SHADER_BYTECODE& BuildVS_Shader() override
	{
		vsBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Line3d.VS.hlsl", L"vs_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return vsBytecode;
	}

	D3D12_SHADER_BYTECODE& BuildPS_Shader(PostEffectType type = PostEffectType::None) override
	{
		psBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Line3d.PS.hlsl", L"ps_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return psBytecode;
	}
};

// ============================================================
// Particle
// ============================================================

class ParticleCompileShader : public CompileShaderBase
{
public:

	ParticleCompileShader(
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	) :CompileShaderBase(dxcUtils, dxcCompiler, includeHandler) {}

	D3D12_SHADER_BYTECODE& BuildVS_Shader() override
	{
		vsBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Particle.VS.hlsl", L"vs_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return vsBytecode;
	}

	D3D12_SHADER_BYTECODE& BuildPS_Shader(PostEffectType type = PostEffectType::None) override
	{
		psBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Particle.PS.hlsl", L"ps_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return psBytecode;
	}
};

// ============================================================
// PrimitiveDrawr
// ============================================================

class PrimitiveDrawrCompileShader : public CompileShaderBase
{
public:

	PrimitiveDrawrCompileShader(
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	) :CompileShaderBase(dxcUtils, dxcCompiler, includeHandler) {}

	D3D12_SHADER_BYTECODE& BuildVS_Shader() override
	{
		vsBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/PrimitiveDrawr.VS.hlsl", L"vs_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return vsBytecode;
	}

	D3D12_SHADER_BYTECODE& BuildPS_Shader(PostEffectType type = PostEffectType::None) override
	{
		psBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/PrimitiveDrawr.PS.hlsl", L"ps_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return psBytecode;
	}
};

// ============================================================
// RenderTexture
// ============================================================

class RenderTextureCompileShader : public CompileShaderBase
{
public:

	RenderTextureCompileShader(
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	) :CompileShaderBase(dxcUtils, dxcCompiler, includeHandler) {}

	D3D12_SHADER_BYTECODE& BuildVS_Shader() override
	{
		vsBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Fullscreen.VS.hlsl", L"vs_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return vsBytecode;
	}

	D3D12_SHADER_BYTECODE& BuildPS_Shader(PostEffectType type = PostEffectType::None) override
	{
		switch (type)
		{
		case PostEffectType::None:
			break;
		case PostEffectType::RenderTexture:
			psBytecode = CompileShaderFactory::CreateCompileShader(
				L"resources/shaders/CopyImage.PS.hlsl", L"ps_6_0",
				dxcUtils_, dxcCompiler_, includeHandler_
			);
			break;
		case PostEffectType::Grayscale:
			psBytecode = CompileShaderFactory::CreateCompileShader(
				L"resources/shaders/Grayscale.PS.hlsl", L"ps_6_0",
				dxcUtils_, dxcCompiler_, includeHandler_
			);
			break;
		case PostEffectType::Vignette:
			psBytecode = CompileShaderFactory::CreateCompileShader(
				L"resources/shaders/Vignette.PS.hlsl", L"ps_6_0",
				dxcUtils_, dxcCompiler_, includeHandler_
			);
			break;
		case PostEffectType::Smoothing:
			psBytecode = CompileShaderFactory::CreateCompileShader(
				L"resources/shaders/GaussianFilter.PS.hlsl", L"ps_6_0",
				dxcUtils_, dxcCompiler_, includeHandler_
			);
			break;
		case PostEffectType::OutLine:
			psBytecode = CompileShaderFactory::CreateCompileShader(
				L"resources/shaders/LuminanceBasedOutline.PS.hlsl", L"ps_6_0",
				dxcUtils_, dxcCompiler_, includeHandler_
			);
			break;
		default:
			break;
		}
		
		return psBytecode;
	}
};

// ============================================================
// Animation
// ============================================================

class AnimationCompileShader : public CompileShaderBase
{
public:

	AnimationCompileShader(
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	) :CompileShaderBase(dxcUtils, dxcCompiler, includeHandler) {}

	D3D12_SHADER_BYTECODE& BuildVS_Shader() override
	{
		vsBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/SkinningObject3d.VS.hlsl", L"vs_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return vsBytecode;
	}

	D3D12_SHADER_BYTECODE& BuildPS_Shader(PostEffectType type = PostEffectType::None) override
	{
		psBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return psBytecode;
	}
};

// ============================================================
// Skybox
// ============================================================

class SkyboxCompileShader : public CompileShaderBase
{
public:

	SkyboxCompileShader(
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	) :CompileShaderBase(dxcUtils, dxcCompiler, includeHandler) {
	}

	D3D12_SHADER_BYTECODE& BuildVS_Shader() override
	{
		vsBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Skybox.VS.hlsl", L"vs_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return vsBytecode;
	}

	D3D12_SHADER_BYTECODE& BuildPS_Shader(PostEffectType type = PostEffectType::None) override
	{
		psBytecode = CompileShaderFactory::CreateCompileShader(
			L"resources/shaders/Skybox.PS.hlsl", L"ps_6_0",
			dxcUtils_, dxcCompiler_, includeHandler_
		);
		return psBytecode;
	}
};

