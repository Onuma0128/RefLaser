#include "DsvManager.h"
#include <cassert>
#include "DirectXEngine.h"
#include "DescriptorHeap.h"

DsvManager* DsvManager::instance_ = nullptr;

DsvManager* DsvManager::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new DsvManager;
	}
	return instance_;
}

void DsvManager::Initialize(DirectXEngine* dxEngine) {
	dxEngine_ = dxEngine;

	descriptorHeap_ = CreateDescriptorHeap(dxEngine_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxDSVCount, false);
	descriptorSize_ = dxEngine_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void DsvManager::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE DsvManager::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += SIZE_T(descriptorSize_ * index);
	return handle;
}

void DsvManager::CreateDSV(uint32_t index, ID3D12Resource* resource) {
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	dxEngine_->GetDevice()->CreateDepthStencilView(resource, &dsvDesc, GetCPUDescriptorHandle(index));
}

void DsvManager::CreateOutlineDSV(uint32_t index, ID3D12Resource* resource)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;

	dxEngine_->GetDevice()->CreateDepthStencilView(resource, &dsvDesc, GetCPUDescriptorHandle(index));
}

uint32_t DsvManager::Allocate() {
	assert(useIndex_ < kMaxDSVCount);
	return useIndex_++;
}
