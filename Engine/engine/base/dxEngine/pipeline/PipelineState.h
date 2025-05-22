#pragma once
#include <Windows.h>
#include "wrl.h"
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <unordered_map>

#include "PipelineStruct.h"
#include "PostEffectType.h"

using Microsoft::WRL::ComPtr;

class PipelineState
{
public:

	void Initialize(
		ComPtr<ID3D12Device>& device, ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3>& dxcCompiler, ComPtr<IDxcIncludeHandler> includeHandler);

private:

	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> CreateRootSignature(PipelineType type, PostEffectType effectType = PostEffectType::None);
	// パイプライン
	ComPtr<ID3D12PipelineState> CreatePipelineState(PipelineType type, PostEffectType effectType = PostEffectType::None, BlendMode blendMode = BlendMode::kBlendModeNormal);

	///==============================================================================================================

	struct PipelineKey {
		PipelineType type;
		PostEffectType effect;
		BlendMode blend;

		bool operator==(const PipelineKey& other) const {
			return type == other.type && effect == other.effect && blend == other.blend;
		}
	};

	struct PipelineKeyHash {
		size_t operator()(const PipelineKey& key) const {
			return std::hash<int>()(static_cast<int>(key.type)) ^
				(std::hash<int>()(static_cast<int>(key.effect)) << 1) ^
				(std::hash<int>()(static_cast<int>(key.blend)) << 2);
		}
	};

	///==============================================================================================================

public:

	ComPtr<ID3D12RootSignature> GetRootSignature(PipelineType type, PostEffectType effectType = PostEffectType::None, BlendMode blend = BlendMode::kBlendModeNormal);

	ComPtr<ID3D12PipelineState> GetPipelineState(PipelineType type, PostEffectType effectType = PostEffectType::None, BlendMode blend = BlendMode::kBlendModeNormal);

private:
	ComPtr<ID3D12Device> device_;
	ComPtr<IDxcUtils> dxcUtils_;
	ComPtr<IDxcCompiler3> dxcCompiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

	std::unordered_map<PipelineKey, ComPtr<ID3D12RootSignature>, PipelineKeyHash> rootSignatures_;
	std::unordered_map<PipelineKey, ComPtr<ID3D12PipelineState>, PipelineKeyHash> pipelineStates_;

	HRESULT hr_{};
};