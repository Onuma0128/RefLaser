#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include "wrl.h"

#include <chrono>

#include "PostEffectType.h"

using Microsoft::WRL::ComPtr;

class DirectXEngine;

class SrvManager
{
private:
	static SrvManager* instance_;

	SrvManager() = default;
	~SrvManager() = default;
	SrvManager(SrvManager&) = delete;
	SrvManager& operator=(SrvManager&) = delete;

public:
	// 最大SRV数
	static const uint32_t kMaxSRVCount_;
	// シングルトンインスタンスの取得
	static SrvManager* GetInstance();

	void Initialize(DirectXEngine* dxEngine);

	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

	void PreDraw();

	void Finalize();


	ID3D12DescriptorHeap* GetDescriptorHeap() { return descriptorHeap_.Get(); }

	// デスクリプタハンドル計算
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	// SRV生成(テクスチャ用)
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	// SRV生成(StructuredBuffer用)
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
	// SRV生成(RenderTexture用)
	void CreateSRVforRenderTexture(uint32_t srvIndex, ID3D12Resource* pResource, PostEffectType type = PostEffectType::None);
	// SRV生成(DDSテクスチャ用)
	void CreateSRVforTextureCube(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

	uint32_t Allocate();

	bool CheckAllocate();

private:

	DirectXEngine* dxEngine_ = nullptr;

	// SRV用のデスクリプタサイズ
	uint32_t descriptorSize_;
	// SRV用のデスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;

	// 次に使用するSRVインデックス
	uint32_t useIndex_ = 0;
};