#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include "wrl.h"

#include <stdint.h>

using Microsoft::WRL::ComPtr;

class DirectXEngine;

class RtvManager
{
private:
	static RtvManager* instance_;
	RtvManager() = default;
	~RtvManager() = default;
	RtvManager(const RtvManager&) = delete;
	RtvManager& operator=(const RtvManager&) = delete;

public:
	static const uint32_t kMaxRTVCount = 256;

	static RtvManager* GetInstance();

	void Initialize(DirectXEngine* dxEngine);
	void Finalize();

	// ハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);

	// RTV作成
	void CreateRTV(uint32_t index, ID3D12Resource* resource);

	// 自動割当
	uint32_t Allocate();

private:
	DirectXEngine* dxEngine_ = nullptr;

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	uint32_t descriptorSize_ = 0;

	uint32_t useIndex_ = 0;
};
