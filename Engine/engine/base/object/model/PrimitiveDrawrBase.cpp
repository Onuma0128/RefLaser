#include "PrimitiveDrawrBase.h"

#include "DirectXEngine.h"
#include "PipelineState.h"

void PrimitiveDrawrBase::Initialize()
{
	rootSignature_ = DirectXEngine::GetPipelineState()->GetRootSignature(PipelineType::PrimitiveDrawr).Get();
	for (int i = 0; i < static_cast<int>(pipelineStates_.size()); ++i) {
		pipelineStates_[i] = DirectXEngine::GetPipelineState()->GetPipelineState(
			PipelineType::PrimitiveDrawr, PostEffectType::None, static_cast<BlendMode>(i)
		).Get();
	}

	skyboxRootSignature_ = DirectXEngine::GetPipelineState()->GetRootSignature(PipelineType::Skybox).Get();
	skyboxPipelineState_ = DirectXEngine::GetPipelineState()->GetPipelineState(PipelineType::Skybox).Get();
}

void PrimitiveDrawrBase::DrawBase(int blendMode)
{
	auto commandList = DirectXEngine::GetCommandList();
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipelineStates_[blendMode].Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void PrimitiveDrawrBase::DrawSkyboxBase()
{
	auto commandList = DirectXEngine::GetCommandList();
	commandList->SetGraphicsRootSignature(skyboxRootSignature_.Get());
	commandList->SetPipelineState(skyboxPipelineState_.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
