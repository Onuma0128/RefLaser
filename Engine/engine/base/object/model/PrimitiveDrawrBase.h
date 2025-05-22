#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include "wrl.h"

#include <array>

using Microsoft::WRL::ComPtr;

class PrimitiveDrawrBase
{
public:

	void Initialize();

	void DrawBase(int blendMode);

	void DrawSkyboxBase();

private:

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	std::array<ComPtr<ID3D12PipelineState>, 5> pipelineStates_;

	ComPtr<ID3D12RootSignature> skyboxRootSignature_ = nullptr;
	ComPtr<ID3D12PipelineState> skyboxPipelineState_;

};