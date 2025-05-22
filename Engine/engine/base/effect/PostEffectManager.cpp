#include "PostEffectManager.h"

#include <algorithm>

#include "WinApp.h"
#include "DirectXEngine.h"
#include "PipelineState.h"

#include "CameraManager.h"
#include "Camera.h"
#include "SrvManager.h"
#include "RtvManager.h"
#include "DsvManager.h"
#include "RenderTexture.h"
#include "CreateBufferResource.h"

void PostEffectManager::Initialize(DirectXEngine* dxEngine)
{
	dxEngine_ = dxEngine;
	pipeline_ = dxEngine->GetPipelineState();

    ResourceInitialize();
}

void PostEffectManager::CreatePostEffect(PostEffectType type)
{
    // 既に登録済みならスキップ
    if (passes_.contains(type)) return;

    PostEffectPass pass;

    // RenderTexture作成
    pass.renderTexture = RenderTexture::CreateResource(
        dxEngine_->GetDevice(),
        WinApp::kClientWidth,
        WinApp::kClientHeight,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        { 0.0f,0.0f,0.0f,1.0f }
    );
    
    // DSV/RTV/SRV 登録
    pass.rtvIndex = RtvManager::GetInstance()->Allocate();
    RtvManager::GetInstance()->CreateRTV(pass.rtvIndex, pass.renderTexture.Get());

    pass.srvIndex = SrvManager::GetInstance()->Allocate() + 1;
    SrvManager::GetInstance()->CreateSRVforRenderTexture(pass.srvIndex, pass.renderTexture.Get());

    if (type == PostEffectType::OutLine) {
        pass.depthSrvIndex = SrvManager::GetInstance()->Allocate() + 1;
        SrvManager::GetInstance()->CreateSRVforRenderTexture(pass.depthSrvIndex, dxEngine_->GetRenderTexrure()->GetDSVResource(), type);
    }

    // PSOとRootSignatureの登録
    rootSignatures_[type] = pipeline_->GetRootSignature(PipelineType::RenderTexture, type, BlendMode::kBlendModeNone).Get();
    pipelineStates_[type] = pipeline_->GetPipelineState(PipelineType::RenderTexture, type, BlendMode::kBlendModeNone).Get();

    // 保存
    enabledEffects_.push_back(type);
    passes_[type] = pass;
}

void PostEffectManager::RenderTextureDraws(uint32_t inputSRVIndex)
{
    Update();

    auto* cmdList = dxEngine_->GetCommandList();
    uint32_t finalSRVIndex = inputSRVIndex;

    for (PostEffectType type : enabledEffects_) {
        const auto& pass = passes_[type];

        D3D12_RESOURCE_BARRIER barrier{};
        if (type == PostEffectType::OutLine) {
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = dxEngine_->GetRenderTexrure()->GetDSVResource();
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            cmdList->ResourceBarrier(1, &barrier);
        }

        // RTVへ遷移
        barrier.Transition.pResource = pass.renderTexture.Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        cmdList->ResourceBarrier(1, &barrier);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = RtvManager::GetInstance()->GetCPUDescriptorHandle(pass.rtvIndex);
        cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        cmdList->SetGraphicsRootSignature(rootSignatures_[type].Get());
        cmdList->SetPipelineState(pipelineStates_[type].Get());
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(0, finalSRVIndex);
        if (type == PostEffectType::OutLine) { SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(1, pass.depthSrvIndex); }
        PostEffectCommand(type);
        cmdList->DrawInstanced(3, 1, 0, 0);

        // 次のエフェクト用にSRVとして使えるように遷移
        barrier.Transition.pResource = pass.renderTexture.Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        cmdList->ResourceBarrier(1, &barrier);

        if (type == PostEffectType::OutLine)
        {
            barrier.Transition.pResource = dxEngine_->GetRenderTexrure()->GetDSVResource();
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            cmdList->ResourceBarrier(1, &barrier);
        }

        // 次のエフェクトの入力として更新
        finalSRVIndex = pass.srvIndex;
    }

    // 最終的なSRVIndexをSwapChain描画に使う
    finalSRVIndex_ = finalSRVIndex;
}

void PostEffectManager::PostEffectCommand(PostEffectType type)
{
    auto* cmdList = dxEngine_->GetCommandList();

    switch (type)
    {
    case PostEffectType::None:
        break;
    case PostEffectType::Grayscale:

        cmdList->SetGraphicsRootConstantBufferView(1, grayscaleResource_->GetGPUVirtualAddress());

        break;
    case PostEffectType::Vignette:

        cmdList->SetGraphicsRootConstantBufferView(1, vignetteResource_->GetGPUVirtualAddress());

        break;
    case PostEffectType::Smoothing:

        break;

    case PostEffectType::OutLine:

        cmdList->SetGraphicsRootConstantBufferView(2, outlineResource_->GetGPUVirtualAddress());

        break;
    default:
        break;
    }
}

void PostEffectManager::ResourceInitialize()
{
    grayscaleResource_ = CreateBufferResource(DirectXEngine::GetDevice(), sizeof(GrayscaleData)).Get();
    grayscaleResource_->Map(0, nullptr, reinterpret_cast<void**>(&grayscaleData_));
    grayscaleData_->color = { 0.2125f, 0.7154f, 0.0721f };
    grayscaleData_->t = 0.0f;

    vignetteResource_ = CreateBufferResource(DirectXEngine::GetDevice(), sizeof(VignetteData)).Get();
    vignetteResource_->Map(0, nullptr, reinterpret_cast<void**>(&vignetteData_));
    vignetteData_->scale = 16.0f;
    vignetteData_->gamma = 0.0f;

    outlineResource_ = CreateBufferResource(DirectXEngine::GetDevice(), sizeof(OutlineData)).Get();
    outlineResource_->Map(0, nullptr, reinterpret_cast<void**>(&outlineData_));
    outlineData_->projection = Matrix4x4::Identity();
}

void PostEffectManager::Update()
{
    grayscaleData_->t = std::clamp(grayscaleData_->t, 0.0f, 2.0f);

    outlineData_->projection = Matrix4x4::Inverse(CameraManager::GetInstance()->GetActiveCamera()->GetProjectionMatrix());
}
