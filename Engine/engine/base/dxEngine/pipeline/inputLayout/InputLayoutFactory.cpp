#include "InputLayoutFactory.h"

#include <cassert>
#include <memory>
#include <unordered_map>

D3D12_INPUT_LAYOUT_DESC& InputLayoutFactory::GetInputLayout(PipelineType type)
{
	static std::unordered_map<PipelineType, std::unique_ptr<InputLayoutBase>> inputLayout;

	switch (type) {
	case PipelineType::Object3d:
		inputLayout[type] = std::make_unique<Object3dInputLayout>();
		break;
	case PipelineType::Sprite:
		inputLayout[type] = std::make_unique<SpriteInputLayout>();
		break;
	case PipelineType::Line3d:
		inputLayout[type] = std::make_unique<Line3dInputLayout>();
		break;
	case PipelineType::Particle:
		inputLayout[type] = std::make_unique<ParticleInputLayout>();
		break;
	case PipelineType::PrimitiveDrawr:
		inputLayout[type] = std::make_unique<PrimitiveDrawrInputLayout>();
		break;
	case PipelineType::Animation:
		inputLayout[type] = std::make_unique<AnimationInputLayout>();
		break;
	case PipelineType::RenderTexture:
		inputLayout[type] = std::make_unique<RenderTextureInputLayout>();
		break;
	case PipelineType::Skybox:
		inputLayout[type] = std::make_unique<SkyboxInputLayout>();
		break;
	default:
		assert(false && "Invalid InputLayoutType");
		break;
	}

	return inputLayout[type]->BuildLayout();
}
