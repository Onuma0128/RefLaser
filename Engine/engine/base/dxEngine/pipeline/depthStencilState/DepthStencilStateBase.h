#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class DepthStencilStateBase
{
public:

	virtual ~DepthStencilStateBase() = default;
	virtual D3D12_DEPTH_STENCIL_DESC& BuildDepthStencil() = 0;

protected:

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc;

};

