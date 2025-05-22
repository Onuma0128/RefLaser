#include "DepthStencilStateFactory.h"

#include <cassert>
#include <memory>
#include <unordered_map>

D3D12_DEPTH_STENCIL_DESC& DepthStencilStateFactory::GetDepthStencilState(PipelineType type)
{
	static std::unordered_map<PipelineType, std::unique_ptr<DepthStencilStateBase>> depthStencilState;

	switch (type) {
	case PipelineType::Object3d:
		depthStencilState[type] = std::make_unique<Object3dDepthStencilState>();
		break;
	case PipelineType::Sprite:
		depthStencilState[type] = std::make_unique<SpriteDepthStencilState>();
		break;
	case PipelineType::Line3d:
		depthStencilState[type] = std::make_unique<Line3dDepthStencilState>();
		break;
	case PipelineType::Particle:
		depthStencilState[type] = std::make_unique<ParticleDepthStencilState>();
		break;
	case PipelineType::PrimitiveDrawr:
		depthStencilState[type] = std::make_unique<PrimitiveDrawrDepthStencilState>();
		break;
	case PipelineType::Animation:
		depthStencilState[type] = std::make_unique<AnimationDepthStencilState>();
		break;
	case PipelineType::RenderTexture:
		depthStencilState[type] = std::make_unique<RenderTextureDepthStencilState>();
		break;
	case PipelineType::Skybox:
		depthStencilState[type] = std::make_unique<SkyboxDepthStencilState>();
		break;
	default:
		assert(false && "Invalid DepthStencilStateType");
		break;
	}

	return depthStencilState[type]->BuildDepthStencil();
}
