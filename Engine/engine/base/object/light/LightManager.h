#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include <memory>

#include "type/DirectionalLight.h"
#include "type/PointLight.h"
#include "type/SpotLight.h"

class DirectXEngine;

class LightManager
{
private:
	static LightManager* instance_;

	LightManager() = default;
	~LightManager() = default;
	LightManager(LightManager&) = delete;
	LightManager& operator=(LightManager&) = delete;

public:

	/*==================== メンバ関数 ====================*/

	// シングルトンインスタンスの取得
	static LightManager* GetInstance();

	void Initialize(DirectXEngine* dxEngine);

	void Update();

	void Debug_ImGui();

	void Finalize();

	/*==================== アクセッサー ====================*/

	ID3D12Resource* GetDirectionalLightResource()const { return directionalLight_->GetResource(); }

	ID3D12Resource* GetPointLightResource()const { return pointLight_->GetResource(); }

	ID3D12Resource* GetSpotLightResource()const { return spotLight_->GetResource(); }

private:

	DirectXEngine* dxEngine_ = nullptr;

	/*==================== 平行光源 ====================*/

	std::unique_ptr<DirectionalLight> directionalLight_ = nullptr;

	/*==================== ポイントライト ====================*/

	std::unique_ptr<PointLight> pointLight_ = nullptr;

	/*==================== スポットライト ====================*/

	std::unique_ptr<SpotLight> spotLight_ = nullptr;

};