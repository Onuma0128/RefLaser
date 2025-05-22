#pragma once
#include <string>
#include <unordered_map>

#include "DirectXTex.h"
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include "wrl.h"

using Microsoft::WRL::ComPtr;

class DirectXEngine;

class TextureManager
{
private:
	static TextureManager* instance_;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

private:
	// テクスチャ一枚分のデータ
	struct TextureData {
		DirectX::TexMetadata metadata;
		ComPtr<ID3D12Resource> resource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

public:
	// シングルトンインスタンスの取得
	static TextureManager* GetInstance();
	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

	// 初期化
	void Initialize(DirectXEngine* dxEngine);
	// 終了
	void Finalize();
    
    // テクスチャの読み込み
	void LoadTexture(const std::string& directoryPath, const std::string& filePath);
	void UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);
	// テクスチャリソースを作成
	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
	// CPU,GPUハンドルの作成
	D3D12_CPU_DESCRIPTOR_HANDLE CreateCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE CreateGPUDescriptorHandle(uint32_t index);

	/*========================== ゲッター ===========================*/

	//// SRVインデックスの開始番号
	//uint32_t GetTextureIndexByFilePath(const std::string& filePath);
	//// テクスチャ番号からGPUハンドルを取得
	//D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);

	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);
	uint32_t GetSrvIndex(const std::string& filePath);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);



private:
	// テクスチャデータ
	std::unordered_map<std::string, TextureData> textureDatas_;
	
	DirectXEngine* dxEngine_ = nullptr;
};