#include "RtvManager.h"
#include <cassert>

#include "DirectXEngine.h"
#include "DescriptorHeap.h"

RtvManager* RtvManager::instance_ = nullptr;

RtvManager* RtvManager::GetInstance()
{
	if (instance_ == nullptr) {
		instance_ = new RtvManager;
	}
	return instance_;
}

void RtvManager::Initialize(DirectXEngine* dxEngine)
{
	dxEngine_ = dxEngine;

	descriptorHeap_ = CreateDescriptorHeap(dxEngine_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxRTVCount, false);
	descriptorSize_ = dxEngine_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void RtvManager::Finalize()
{
	delete instance_;
	instance_ = nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE RtvManager::GetCPUDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (SIZE_T)(descriptorSize_ * index);
	return handle;
}

void RtvManager::CreateRTV(uint32_t index, ID3D12Resource* resource)
{
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	dxEngine_->GetDevice()->CreateRenderTargetView(resource, &rtvDesc, GetCPUDescriptorHandle(index));
}

uint32_t RtvManager::Allocate()
{
	assert(useIndex_ < kMaxRTVCount);
	return useIndex_++;
}
