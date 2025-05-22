#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <dxcapi.h>
#include <array>
#include <chrono>

#include "StringUtility.h"
#include "PipelineState.h"
#include "PostEffectManager.h"
#include "RenderTexture.h"
#include "SceneRenderer.h"
#include "CollisionManager.h"

class WinApp;
class ImGuiManager;

using Microsoft::WRL::ComPtr;

class DirectXEngine
{
public:

	~DirectXEngine();

	// 初期化
	void Initialize(WinApp* winApp, ImGuiManager* imguiManager);
	// デバイスの初期化
	void DeviceInitialize();
	// コマンド関連の初期化
	void CommandInitialize();
	// スワップチェーンの初期化
	void SwapChainInitialize();
	// 各種デスクリプタヒープの初期化
	void DescriptorHeapInitialize();
	// レンダーターゲットビューの初期化
	void RTVInitialize();
	// フェンスの初期化
	void FenceInitialize();
	// ビューポートの初期化
	void ViewportInitialize();
	// シザー矩形の初期化
	void RectInitialize();
	// DXCコンパイラの初期化
	void DxcCompilerInitialize();
	// IncludeHandlerの初期化
	void IncludeHandlerInitialize();
	// PipelineStateの初期化
	void PipelineStateInitialize();
	// FPS固定の初期化
	void InitializeFixFPS();
	// RenderTextureの初期化
	void RenderTextureInitialize();
	// FPS固定の更新
	void UpdateFixFPS();

	// 描画前の処理
	void PreDraw();
	// Object描画後の処理
	void SwapChainDrawSet();
	// 描画後の処理
	void PostDraw();

	/*========================== ゲッター ===========================*/

	// バックバッファの数を取得
	size_t GetBackBufferCount()const { return static_cast<size_t>(swapChainDesc_.BufferCount); }

	// デバイス
	static ID3D12Device* GetDevice() { return device_.Get(); }
	// コマンドリスト
	static ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }
	// パイプライン
	static PipelineState* GetPipelineState() { return pipelineState_.get(); }
	// ポストエフェクト
	static PostEffectManager* GetPostEffectMgr() { return postEffectManager_.get(); }
	// 全ての描画
	static SceneRenderer* GetSceneRenderer() { return sceneRendrer_.get(); }
	// 当たり判定
	static CollisionManager* GetCollisionMgr() { return collisionManager_.get(); }
	// RenderTexture
	RenderTexture* GetRenderTexrure() { return renderTexture_.get(); }

private:
	// StringUtility
	std::unique_ptr<StringUtility> stringUtility_ = nullptr;
	// WindowsAPI
	WinApp* winApp_ = nullptr;
	// RenderTexture
	std::unique_ptr<RenderTexture> renderTexture_ = nullptr;
	// PipelineState
	static std::unique_ptr<PipelineState> pipelineState_;
	// PostEffectManager
	static std::unique_ptr<PostEffectManager> postEffectManager_;
	// D3D12Deviceの作成
	static ComPtr<ID3D12Device> device_;
	// commandListを生成する
	static ComPtr<ID3D12GraphicsCommandList> commandList_;
	// SceneRendrer
	static std::unique_ptr<SceneRenderer> sceneRendrer_;
	// CollisionManager
	static std::unique_ptr<CollisionManager> collisionManager_;

	///==============================================================

	// DXGIファクトリーの生成
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	//コマンドキューを生成する
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	//コマンドアロケータを生成する
	ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//スワップチェーンを生成する
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	std::array<ComPtr<ID3D12Resource>, 2> swapChainResources_ = { nullptr };
	//RTVを2つ作るのでディスクリプタを2つ用意
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 2> rtvHandles_{};
	// フェンスを生成
	ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_{};
	// ビューポートの生成
	D3D12_VIEWPORT viewport_{};
	// シザー矩形の生成
	D3D12_RECT scissorRect_{};
	// DxcCompilerの生成
	ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	// IncludeHandlerの生成
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
	// TransitionBarrierの生成
	D3D12_RESOURCE_BARRIER barrier_{};
	// 記録時間
	std::chrono::steady_clock::time_point reference_;
};