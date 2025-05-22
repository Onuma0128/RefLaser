#include "RootSignatureFactory.h"

#include <cassert>
#include <memory>
#include <unordered_map>

#include "Object3dRootSignature.h"
#include "SpriteRootSignature.h"
#include "Line3dRootSignature.h"
#include "ParticleRootSignature.h"
#include "PrimitiveDrawrRootSignature.h"
#include "AnimationRootSignature.h"
#include "RenderTextureRootSignature.h"
#include "SkyboxRootSignature.h"

ComPtr<ID3D12RootSignature> RootSignatureFactory::GetRootSignature(PipelineType type, ComPtr<ID3D12Device> device, PostEffectType effectType)
{
	static std::unordered_map<PipelineType, std::unique_ptr<RootSignatureBase>> rootSignature;

	switch (type) {
	case PipelineType::Object3d:
		rootSignature[type] = std::make_unique<Object3dRootSignature>();
		break;
	case PipelineType::Sprite:
		rootSignature[type] = std::make_unique<SpriteRootSignature>();
		break;
	case PipelineType::Line3d:
		rootSignature[type] = std::make_unique<Line3dRootSignature>();
		break;
	case PipelineType::Particle:
		rootSignature[type] = std::make_unique<ParticleRootSignature>();
		break;
	case PipelineType::PrimitiveDrawr:
		rootSignature[type] = std::make_unique<PrimitiveDrawrRootSignature>();
		break;
	case PipelineType::Animation:
		rootSignature[type] = std::make_unique<AnimationRootSignature>();
		break;
	case PipelineType::RenderTexture:
		rootSignature[type] = std::make_unique<RenderTextureRootSignature>();
		break;
	case PipelineType::Skybox:
		rootSignature[type] = std::make_unique<SkyboxRootSignature>();
		break;
	default:
		assert(false && "Invalid RootSignatureType");
		break;
	}

	return std::move(rootSignature[type]->BuildRootSignature(device.Get(), effectType));
}
