#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include <array>

class InputLayoutBase
{
public:

	virtual ~InputLayoutBase() = default;
	virtual D3D12_INPUT_LAYOUT_DESC& BuildLayout() = 0;

protected:

	std::array<D3D12_INPUT_ELEMENT_DESC, 10> descs;
	D3D12_INPUT_LAYOUT_DESC layoutDesc{};

};
