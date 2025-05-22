#include "DirectXEngine.h"
#include <cassert>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include <format>
#include <thread>

#include "Logger.h"
#include "WinApp.h"

#include "ImGuiManager.h"
#include "SrvManager.h"
#include "RtvManager.h"
#include "DsvManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "AudioManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ParticleManager.h"

using Microsoft::WRL::ComPtr;

ComPtr<ID3D12Device> DirectXEngine::device_ = nullptr;
ComPtr<ID3D12GraphicsCommandList> DirectXEngine::commandList_ = nullptr;
std::unique_ptr<PipelineState> DirectXEngine::pipelineState_ = nullptr;
std::unique_ptr<PostEffectManager> DirectXEngine::postEffectManager_ = nullptr;
std::unique_ptr<SceneRenderer> DirectXEngine::sceneRendrer_ = nullptr;
std::unique_ptr<CollisionManager> DirectXEngine::collisionManager_ = nullptr;

DirectXEngine::~DirectXEngine()
{
	SrvManager::GetInstance()->Finalize();
	RtvManager::GetInstance()->Finalize();
	DsvManager::GetInstance()->Finalize();
	CameraManager::GetInstance()->Finalize();
	TextureManager::GetInstance()->Finalize();
	LightManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	ParticleManager::GetInstance()->Finalize();
	AudioManager::GetInstance()->Finalize();

	device_.Reset();
	commandList_.Reset();
	pipelineState_.reset();
	postEffectManager_.reset();
	sceneRendrer_->Finalize();
	sceneRendrer_.reset();
	collisionManager_.reset();

	//解放の処理
	CloseHandle(fenceEvent_);
}

void DirectXEngine::Initialize(WinApp* winApp, ImGuiManager* imguiManager)
{
	assert(winApp);
	winApp_ = winApp;

	// FPS固定の初期化
	InitializeFixFPS();
	// デバイスの初期化
	DeviceInitialize();
	// コマンド関連の初期化
	CommandInitialize();
	// スワップチェーンの初期化
	SwapChainInitialize();
	// 各種デスクリプタヒープの初期化
	DescriptorHeapInitialize();

	/*==================== ImGui ====================*/

	imguiManager->Initialize(this, winApp_);

	// レンダーターゲットビューの初期化
	RTVInitialize();
	// フェンスの初期化
	FenceInitialize();
	// ビューポートの初期化
	ViewportInitialize();
	// シザー矩形の初期化
	RectInitialize();
	// DXCコンパイラの初期化
	DxcCompilerInitialize();
	// IncludeHandlerの初期化
	IncludeHandlerInitialize();
	// PipelineStateの初期化
	PipelineStateInitialize();
	// RenderTextureの初期化
	RenderTextureInitialize();

	/*==================== カメラ準備用 ====================*/

	CameraManager::GetInstance()->Initialize(this);

	/*==================== ライト準備用 ====================*/

	LightManager::GetInstance()->Initialize(this);

	/*==================== テクスチャ読み込み ====================*/

	TextureManager::GetInstance()->Initialize(this);

	/*==================== モデル読み込み ====================*/

	ModelManager::GetInstance()->Initialize(this);

	/*==================== パーティクル ====================*/

	ParticleManager::GetInstance()->Initialize(this);

	postEffectManager_ = std::make_unique<PostEffectManager>();
	postEffectManager_->Initialize(this);

	sceneRendrer_ = std::make_unique<SceneRenderer>();

	collisionManager_ = std::make_unique<CollisionManager>();
}

void DirectXEngine::DeviceInitialize()
{
	//デバックレイヤー
#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバックレイヤーを有効化にする
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // DEBUG

	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapteDesc{};
		hr = useAdapter->GetDesc3(&adapteDesc);
		assert(SUCCEEDED(hr));
		//ソフトウェアアダプタでなければ採用
		if (!(adapteDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力。wstringの方なので注意
			Logger::Log(stringUtility_->ConvertString(std::format(L"Use Adapater : {}\n", adapteDesc.Description)));
			//Log(std::format(L"Use Adapater : {}\n", adapteDesc.Description));
			break;
		}
		useAdapter = nullptr; //ソフトウェアアダプタの場合は見えなかったことにする
	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける
			Logger::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	Logger::Log("Complete create D3D12Device!!!\n");
#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//解放
		infoQueue->Release();
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバックレイヤーとDX12デバックレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif // DEBUG
}

void DirectXEngine::CommandInitialize()
{
	HRESULT hr{};

	//コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));

	//コマンドアロケータを生成する
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr));

	//コマンドリストを生成する
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
}

void DirectXEngine::SwapChainInitialize()
{
	HRESULT hr{};

	//スワップチェーンを生成する
	swapChainDesc_.Width = winApp_->kClientWidth; //画面の幅
	swapChainDesc_.Height = winApp_->kClientHeight; //画面の高さ
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //色の形式
	swapChainDesc_.SampleDesc.Count = 1; //マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2; //ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //モニタにうつしたら、中身を破棄
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetHwnd(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

void DirectXEngine::DescriptorHeapInitialize()
{
	/*==================== SRV,RTV,DSV ====================*/

	SrvManager::GetInstance()->Initialize(this);
	RtvManager::GetInstance()->Initialize(this);
	DsvManager::GetInstance()->Initialize(this);
}

void DirectXEngine::RTVInitialize()
{
	HRESULT hr{};

	// swapChainを2つ初期化
	for (uint32_t i = 0; i < 2; ++i) {
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
		assert(SUCCEEDED(hr));

		// RtvManagerでRTVを割り当てて作成
		uint32_t rtvIndex = RtvManager::GetInstance()->Allocate();
		rtvHandles_[i] = RtvManager::GetInstance()->GetCPUDescriptorHandle(rtvIndex);
		RtvManager::GetInstance()->CreateRTV(rtvIndex, swapChainResources_[i].Get());
	}
}

void DirectXEngine::FenceInitialize()
{
	HRESULT hr{};

	//初期値0でFenceを作る
	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));
	//FenceのSignalを持つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXEngine::ViewportInitialize()
{
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = winApp_->kClientWidth;
	viewport_.Height = winApp_->kClientHeight;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

void DirectXEngine::RectInitialize()
{
	//基本的にビューポート同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = winApp_->kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = winApp_->kClientHeight;
}

void DirectXEngine::DxcCompilerInitialize()
{
	HRESULT hr{};

	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));
}

void DirectXEngine::IncludeHandlerInitialize()
{
	HRESULT hr{};

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

void DirectXEngine::PipelineStateInitialize()
{
	// 新しいパイプライン
	pipelineState_ = std::make_unique<PipelineState>();
	pipelineState_->Initialize(device_, dxcUtils_, dxcCompiler_, includeHandler_);
}

void DirectXEngine::InitializeFixFPS()
{
	reference_ = std::chrono::steady_clock::now();
}

void DirectXEngine::RenderTextureInitialize()
{
	renderTexture_ = std::make_unique<RenderTexture>();
	renderTexture_->Initialize();
}

void DirectXEngine::UpdateFixFPS()
{
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));
	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒 (よりわずかに短い時間) 経っていない場合
	if (elapsed < kMinCheckTime) {

		// 1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void DirectXEngine::PreDraw()
{
	// RenderTextureのStartBarrier
	renderTexture_->StartBarrier();
	// RenderTextureの描画前準備
	renderTexture_->PreDraw();
	//描画用のDescriptorHeapの設定
	SrvManager::GetInstance()->PreDraw();
	//コマンドを積む
	commandList_->RSSetViewports(1, &viewport_);
	commandList_->RSSetScissorRects(1, &scissorRect_);
}

void DirectXEngine::SwapChainDrawSet()
{
	// RenderTextureのEndBarrier
	renderTexture_->EndBarrier();

	// PostEffectの複数描画
	postEffectManager_->RenderTextureDraws(renderTexture_->GetRenderTextureSRVIndex());

	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = swapChainResources_[backBufferIndex].Get();
	//遷移前(現在)のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);

	// これから書き込むバックバッファのインデックスを取得
	//描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = renderTexture_->GetDSVHandle();
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle);
	//コマンドを積む
	commandList_->RSSetViewports(1, &viewport_);
	commandList_->RSSetScissorRects(1, &scissorRect_);

	// RenderTextureの描画
	renderTexture_->Draw(postEffectManager_->GetFinalSRVIndex());
}

void DirectXEngine::PostDraw()
{
	HRESULT hr{};
	//画面に描く処理は全て終わり、画面に映すので、状態を遷移
	//今回はRenderTargetからPresentにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);
	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));
	//GPUにコマンドリストの実行を行わせる
	ComPtr<ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	//GPUとOSに画面の交換を行うよう通知をする
	swapChain_->Present(1, 0);
	//FenceValue値を更新
	++fenceValue_;
	//GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	//Fenceの値が指定したSignal値にたどりついているか確認する
	//GetCompleteValueの初期値はFence作成時に渡した初期化
	if (fence_->GetCompletedValue() < fenceValue_) {
		//指定したSignelにたどり着いてないので、たどり着くまで待つようにイベントを設定
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
	// FPS固定
	UpdateFixFPS();
	//次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}