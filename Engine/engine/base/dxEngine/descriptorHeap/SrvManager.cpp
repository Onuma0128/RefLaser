#include "SrvManager.h"

#include <cassert>

#include "DirectXEngine.h"
#include "DescriptorHeap.h"

const uint32_t SrvManager::kMaxSRVCount_ = 512;

SrvManager* SrvManager::instance_ = nullptr;

SrvManager* SrvManager::GetInstance()
{
	if (instance_ == nullptr) {
		instance_ = new SrvManager;
	}
	return instance_;
}

void SrvManager::Initialize(DirectXEngine* dxEngine)
{
	dxEngine_ = dxEngine;

	// デスクリプタヒープの生成
	descriptorHeap_ = CreateDescriptorHeap(dxEngine_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount_, true);
	// デスクリプタ1個分のサイズを取得して記録
	descriptorSize_ = dxEngine_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void SrvManager::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex)
{
	dxEngine_->GetCommandList()->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}

void SrvManager::PreDraw()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
	dxEngine_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void SrvManager::Finalize()
{
	delete instance_;
	instance_ = nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize_ * index);
	return handleGPU;
}

void SrvManager::CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2DTexture
	srvDesc.Texture2D.MipLevels = UINT(MipLevels);
	// SRVの生成
	dxEngine_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvManager::CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.StructureByteStride = structureByteStride;
	// SRVの生成
	dxEngine_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvManager::CreateSRVforRenderTexture(uint32_t srvIndex, ID3D12Resource* pResource, PostEffectType type)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	// typeがOutlineならFormatを変える
	if (type == PostEffectType::OutLine) {
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	} else {
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2DTexture
	srvDesc.Texture2D.MipLevels = 1;
	// SRVの生成
	dxEngine_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvManager::CreateSRVforTextureCube(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE; //DDS2DTexture
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = UINT(MipLevels);
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	// SRVの生成
	dxEngine_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

uint32_t SrvManager::Allocate()
{
	assert(useIndex_ < kMaxSRVCount_);

	int index = useIndex_;
	useIndex_++;
	return index;
}

bool SrvManager::CheckAllocate()
{
	if (useIndex_ < kMaxSRVCount_) {
		return true;
	}
	return false;
}
