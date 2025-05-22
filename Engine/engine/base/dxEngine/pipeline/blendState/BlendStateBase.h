#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class BlendStateBase
{
public:

	virtual ~BlendStateBase() = default;
	virtual D3D12_BLEND_DESC& BuildBlend() = 0;

protected:

	D3D12_BLEND_DESC blendDesc;

};

