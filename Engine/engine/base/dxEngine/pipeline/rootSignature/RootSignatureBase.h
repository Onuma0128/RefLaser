#pragma once
#include "wrl.h"
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include "PostEffectType.h"

using Microsoft::WRL::ComPtr;

class RootSignatureBase
{
public:

	virtual ~RootSignatureBase() = default;
	virtual ComPtr<ID3D12RootSignature> BuildRootSignature(ID3D12Device* device, PostEffectType effectType = PostEffectType::None) = 0;

protected:

	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr{};

};

