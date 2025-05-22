#include "Line3dBase.h"

#include "DirectXEngine.h"
#include "PipelineState.h"

void Line3dBase::Initialize()
{
	rootSignature_ = DirectXEngine::GetPipelineState()->GetRootSignature(PipelineType::Line3d).Get();
	pipelineState_ = DirectXEngine::GetPipelineState()->GetPipelineState(PipelineType::Line3d).Get();
}

void Line3dBase::DrawBase()
{
	auto commandList = DirectXEngine::GetCommandList();
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipelineState_.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}
