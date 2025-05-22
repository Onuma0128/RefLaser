#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class RasterizerStateBase
{
public:

	virtual ~RasterizerStateBase() = default;
	virtual D3D12_RASTERIZER_DESC& BuildRasterizer() = 0;

protected:

	D3D12_RASTERIZER_DESC rasterizerDesc;

};

