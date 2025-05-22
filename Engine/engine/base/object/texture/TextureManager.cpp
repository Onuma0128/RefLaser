#include "TextureManager.h"

#include "DirectXEngine.h"
#include "SrvManager.h"
#include "LoadTexture.h"

TextureManager* TextureManager::instance_ = nullptr;
uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance()
{
	if (instance_ == nullptr) {
		instance_ = new TextureManager;
	}
	return instance_;
}

void TextureManager::Initialize(DirectXEngine* dxEngine)
{
	textureDatas_.reserve(SrvManager::kMaxSRVCount_);
	dxEngine_ = dxEngine;
}

void TextureManager::Finalize()
{
	delete instance_;
	instance_ = nullptr;
}

void TextureManager::LoadTexture(const std::string& directoryPath, const std::string& filePath)
{
	if(textureDatas_.contains(filePath)){
		// 読み込み済みなら早期return
		return;
	}

	// テクスチャファイルを読んでプログラムで抑えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(directoryPath + "/" + filePath);
	HRESULT hr;
	if (filePathW.ends_with(L".dds")) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	} else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	const DirectX::TexMetadata& metadata = image.GetMetadata();
	TextureData& textureData = textureDatas_[filePath];
	if (metadata.width <= 1 && metadata.height <= 1) {
		// テクスチャデータを追加
		textureData.metadata = image.GetMetadata();
		textureData.resource = CreateTextureResource(textureData.metadata);
		UploadTextureData(textureData.resource, image);
	}
	else {
		DirectX::ScratchImage mipImages{};
		if (DirectX::IsCompressed(image.GetMetadata().format)) {
			mipImages = std::move(image);
		} else {
			hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
		}
		assert(SUCCEEDED(hr));

		// テクスチャデータを追加
		textureData.metadata = mipImages.GetMetadata();
		textureData.resource = CreateTextureResource(textureData.metadata);
		UploadTextureData(textureData.resource, mipImages);
	}

	// テクスチャデータの要素数番号
	textureData.srvIndex = SrvManager::GetInstance()->Allocate() + kSRVIndexTop;
	// CPU,GPUハンドルを取得
	textureData.srvHandleCPU = SrvManager::GetInstance()->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = SrvManager::GetInstance()->GetGPUDescriptorHandle(textureData.srvIndex);

	// SRVの設定
	if (metadata.IsCubemap()) {
		SrvManager::GetInstance()->CreateSRVforTextureCube(
			textureData.srvIndex,
			textureData.resource.Get(),
			textureData.metadata.format,
			UINT(textureData.metadata.mipLevels)
		);
	} else {
		SrvManager::GetInstance()->CreateSRVforTexture2D(
			textureData.srvIndex,
			textureData.resource.Get(),
			textureData.metadata.format,
			UINT(textureData.metadata.mipLevels)
		);
	}
	
	// テクスチャ枚数上限チェック
	assert(SrvManager::GetInstance()->CheckAllocate());
}

void TextureManager::UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages)
{
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapについて
	for (size_t arraySlice = 0; arraySlice < metadata.arraySize; ++arraySlice) {
		for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
			const DirectX::Image* img = mipImages.GetImage(mipLevel, arraySlice, 0);
			HRESULT hr = texture->WriteToSubresource(
				UINT(mipLevel + arraySlice * metadata.mipLevels),
				nullptr,
				img->pixels,
				UINT(img->rowPitch),
				UINT(img->slicePitch)
			);
			assert(SUCCEEDED(hr));
		}
	}
}

ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
	// 1.metadeを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); //Textureの幅
	resourceDesc.Height = UINT(metadata.height); //Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); //mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); //奥行きor配列Textureの配列数
	resourceDesc.Format = metadata.format; //TextureのFormat
	resourceDesc.SampleDesc.Count = 1; //サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); //Textureの次元数。普段使ってるのは2次元
	// 2.利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; //細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; //writeBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; //プロセッサの近くに配置
	// 3.Resourceを生成する
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = dxEngine_->GetDevice()->CreateCommittedResource(
		&heapProperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::CreateCPUDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = SrvManager::GetInstance()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	uint32_t descriptorSize = dxEngine_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::CreateGPUDescriptorHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = SrvManager::GetInstance()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	uint32_t descriptorSize = dxEngine_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

//uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
//{
//	auto it = std::find_if(
//		textureDatas_.begin(),
//		textureDatas_.end(),
//		[&](TextureData& textureData) {return textureData.filePath == filePath; }
//	);
//	if (it != textureDatas_.end()) {
//		// 読み込み済みなら要素番号を返す
//		uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas_.begin(), it));
//		return textureIndex;
//	}
//
//	assert(0);
//	return 0;
//}
//
//D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex)
//{
//	// 範囲外指定違反チェック
//	assert(textureIndex < textureDatas_.size());
//
//	TextureData& textureData = textureDatas_[textureIndex];
//	return textureData.srvHandleGPU;
//}

// メタデータを取得
const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath) {
	auto it = textureDatas_.find(filePath);
	if (it != textureDatas_.end()) {
		return it->second.metadata;
	}
	else {
		throw std::runtime_error("Texture not found: " + filePath);
	}
}

// SRVインデックスを取得
uint32_t TextureManager::GetSrvIndex(const std::string& filePath) {
	auto it = textureDatas_.find(filePath);
	if (it != textureDatas_.end()) {
		return it->second.srvIndex;
	}
	else {
		throw std::runtime_error("Texture not found: " + filePath);
	}
}

// GPU側のSRVハンドルを取得
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath) {
	auto it = textureDatas_.find(filePath);
	if (it != textureDatas_.end()) {
		return it->second.srvHandleGPU;
	}
	else {
		throw std::runtime_error("Texture not found: " + filePath);
	}
}
