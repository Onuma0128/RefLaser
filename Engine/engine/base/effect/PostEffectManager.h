#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include "wrl.h"

#include <stdint.h>
#include <vector>
#include <unordered_map>

#include "PostEffectType.h"

using Microsoft::WRL::ComPtr;

class DirectXEngine;
class PipelineState;

class PostEffectManager
{
public:

	void Initialize(DirectXEngine* dxEngine);

    void CreatePostEffect(PostEffectType type);

	// ポストエフェクト全ての描画
    void PostEffectCommand(PostEffectType type);
    void RenderTextureDraws(uint32_t inputSRVIndex);
    // 最後のSRVIndexを保管
    uint32_t GetFinalSRVIndex()const { return finalSRVIndex_; }

    // ポストエフェクト全てのポインタ
    // GrayscaleData
    GrayscaleData* GetGrayscaleData() { return grayscaleData_; }
    // VignetteData
    VignetteData* GetVignetteData() { return vignetteData_; }

private:

    void ResourceInitialize();

    void Update();

private:

    struct PostEffectPass {
        ComPtr<ID3D12Resource> renderTexture;
        uint32_t srvIndex = 0;
        uint32_t rtvIndex = 0;
        uint32_t depthSrvIndex = 0;
    };

    DirectXEngine* dxEngine_ = nullptr;
    PipelineState* pipeline_ = nullptr;

    std::vector<PostEffectType> enabledEffects_;
    std::unordered_map<PostEffectType, PostEffectPass> passes_;
    std::unordered_map<PostEffectType, ComPtr<ID3D12PipelineState>> pipelineStates_;
    std::unordered_map<PostEffectType, ComPtr<ID3D12RootSignature>> rootSignatures_;

    uint32_t finalSRVIndex_;

    // GrayscaleData
    ComPtr<ID3D12Resource> grayscaleResource_ = nullptr;
    GrayscaleData* grayscaleData_;
    // VignetteData
    ComPtr<ID3D12Resource> vignetteResource_ = nullptr;
    VignetteData* vignetteData_;
    // OutlineData
    ComPtr<ID3D12Resource> outlineResource_ = nullptr;
    OutlineData* outlineData_;
};

