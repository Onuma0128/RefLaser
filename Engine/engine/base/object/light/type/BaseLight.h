#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class DirectXEngine;

class BaseLight
{
public:

	virtual void Initialize(DirectXEngine* dxEngine) = 0;

	virtual void Update() = 0;

	virtual void Debug_ImGui() = 0;

	virtual void MakeLightData() = 0;

	ID3D12Resource* GetResource()const { return resource_.Get(); }

protected:

	/*==================== ライト用のデータ ====================*/

	// Engineのポインタ
	DirectXEngine* dxEngine_ = nullptr;
	// バッファリソース
	ComPtr<ID3D12Resource> resource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	D3D12_VERTEX_BUFFER_VIEW bufferView_{};

};