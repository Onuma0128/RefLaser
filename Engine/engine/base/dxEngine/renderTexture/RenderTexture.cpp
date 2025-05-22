#include "RenderTexture.h"

#include <cassert>

#include "DirectXEngine.h"
#include "WinApp.h"
#include "SrvManager.h"
#include "RtvManager.h"
#include "DsvManager.h"

#include "DepthStencilTexture.h"

ComPtr<ID3D12Resource> RenderTexture::CreateResource(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor)
{
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;										// Textureの幅
	resourceDesc.Height = height;									// Textureの高さ
	resourceDesc.MipLevels = 1;										// mipmapの数
	resourceDesc.DepthOrArraySize = 1;								// 奥行きor配列Textureの配列数
	resourceDesc.Format = format;									// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;								// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;	// RenderTargetとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;				// フォーマット。Resourceと合わせる
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;

	// Resourceの生成
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

ID3D12Resource* RenderTexture::GetDSVResource() const
{
	return depthStencilResource_.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTexture::GetDSVHandle() const
{
	return DsvManager::GetInstance()->GetCPUDescriptorHandle(depthIndex_);
}

void RenderTexture::Initialize()
{
	// 深度リソースを作成
	depthStencilResource_ = CreateDepthStencilTextureResource(
		DirectXEngine::GetDevice(),
		WinApp::kClientWidth,
		WinApp::kClientHeight
	);
	// DSVManagerからインデックスを割り当て
	depthIndex_ = DsvManager::GetInstance()->Allocate();
	DsvManager::GetInstance()->CreateDSV(depthIndex_, depthStencilResource_.Get());

	// RenderTextureのRTVの設定
	const Vector4 kRenderTargetClearValue = { 0.0f,0.0f,0.2f,1.0f };
	renderTextureResource_ = RenderTexture::CreateResource(
		DirectXEngine::GetDevice(),
		WinApp::kClientWidth,
		WinApp::kClientHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		kRenderTargetClearValue
	);
	// RtvManagerでRTVを割り当てて作成
	uint32_t renderTextureRTVIndex = RtvManager::GetInstance()->Allocate();
	renderTextureHandle_ = RtvManager::GetInstance()->GetCPUDescriptorHandle(renderTextureRTVIndex);
	RtvManager::GetInstance()->CreateRTV(renderTextureRTVIndex, renderTextureResource_.Get());

	// RenderTextureのSRVの設定
	renderTextureSRVIndex_ = SrvManager::GetInstance()->Allocate() + 1;
	SrvManager::GetInstance()->CreateSRVforRenderTexture(renderTextureSRVIndex_, renderTextureResource_.Get());

	// ルートシグネチャ,パイプラインステート
	rootSignature_ = DirectXEngine::GetPipelineState()->GetRootSignature(
		PipelineType::RenderTexture, PostEffectType::RenderTexture, BlendMode::kBlendModeNone
	).Get();
	pipelineState_ = DirectXEngine::GetPipelineState()->GetPipelineState(
		PipelineType::RenderTexture, PostEffectType::RenderTexture, BlendMode::kBlendModeNone
	).Get();
}

void RenderTexture::StartBarrier()
{
	//TransitionBarrierの設定
	//今回のバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = renderTextureResource_.Get();
	//遷移前(現在)のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	auto* commandList = DirectXEngine::GetCommandList();
	commandList->ResourceBarrier(1, &barrier_);
}

void RenderTexture::EndBarrier()
{
	//遷移前(現在)のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//TransitionBarrierを張る
	auto* commandList = DirectXEngine::GetCommandList();
	commandList->ResourceBarrier(1, &barrier_);
}

void RenderTexture::PreDraw()
{
	//描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DsvManager::GetInstance()->GetCPUDescriptorHandle(depthIndex_);
	auto* commandList = DirectXEngine::GetCommandList();
	commandList->OMSetRenderTargets(1, &renderTextureHandle_, false, &dsvHandle);
	//指定した深度で画面全体をクリアする
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.0f,0.0f,0.2f,1.0f }; // 青色。RGBAの順
	commandList->ClearRenderTargetView(renderTextureHandle_, clearColor, 0, nullptr);
}

void RenderTexture::Draw(uint32_t finalSRVIndex)
{
	auto* commandList = DirectXEngine::GetCommandList();
	// RenderTextureの描画
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipelineState_.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(0, finalSRVIndex);
	commandList->DrawInstanced(3, 1, 0, 0);
}
