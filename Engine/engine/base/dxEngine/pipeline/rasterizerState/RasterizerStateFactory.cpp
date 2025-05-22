#include "RasterizerStateFactory.h"

#include <cassert>
#include <memory>
#include <unordered_map>

D3D12_RASTERIZER_DESC& RasterizerStateFactory::GetRasterizerDesc(PipelineType type)
{
	static std::unordered_map<PipelineType,std::unique_ptr<RasterizerStateBase>> rasterizerState;

	switch (type) {
	case PipelineType::Object3d:
		rasterizerState[type] = std::make_unique<Object3dRasterizerState>();
		break;
	case PipelineType::Sprite:
		rasterizerState[type] = std::make_unique<SpriteRasterizerState>();
		break;
	case PipelineType::Line3d:
		rasterizerState[type] = std::make_unique<Line3dRasterizerState>();
		break;
	case PipelineType::Particle:
		rasterizerState[type] = std::make_unique<ParticleRasterizerState>();
		break;
	case PipelineType::PrimitiveDrawr:
		rasterizerState[type] = std::make_unique<PrimitiveDrawrRasterizerState>();
		break;
	case PipelineType::Animation:
		rasterizerState[type] = std::make_unique<AnimationRasterizerState>();
		break;
	case PipelineType::RenderTexture:
		rasterizerState[type] = std::make_unique<RenderTextureRasterizerState>();
		break;
	case PipelineType::Skybox:
		rasterizerState[type] = std::make_unique<SkyboxRasterizerState>();
		break;
	default:
		assert(false && "Invalid RasterizerStateType");
		break;
	}

	return rasterizerState[type]->BuildRasterizer();
}
