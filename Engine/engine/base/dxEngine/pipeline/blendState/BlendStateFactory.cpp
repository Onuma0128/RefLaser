#include "BlendStateFactory.h"

#include <cassert>
#include <memory>
#include <unordered_map>

D3D12_BLEND_DESC& BlendStateFactory::GetBlendState(BlendMode type)
{
	static std::unordered_map<BlendMode, std::unique_ptr<BlendStateBase>> blendState;

	switch (type)
	{
	case BlendMode::kBlendModeNone:
		blendState[type] = std::make_unique<BlendStateNone>();
		break;
	case BlendMode::kBlendModeNormal:
		blendState[type] = std::make_unique<BlendStateNormal>();
		break;
	case BlendMode::kBlendModeAdd:
		blendState[type] = std::make_unique<BlendStateAdd>();
		break;
	case BlendMode::kBlendModeSubtract:
		blendState[type] = std::make_unique<BlendStateSubtract>();
		break;
	case BlendMode::kBlendModeMultily:
		blendState[type] = std::make_unique<BlendStateMultily>();
		break;
	default:
		assert(false && "Invalid BlendMode");
		break;
	}

	return blendState[type]->BuildBlend();
}
