#include "LightManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

#include "DirectXEngine.h"

LightManager* LightManager::instance_ = nullptr;

LightManager* LightManager::GetInstance()
{
	if (instance_ == nullptr) {
		instance_ = new LightManager;
	}
	return instance_;
}

void LightManager::Initialize(DirectXEngine* dxEngine)
{
	this->dxEngine_ = dxEngine;

	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(dxEngine_);

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize(dxEngine_);

	spotLight_ = std::make_unique<SpotLight>();
	spotLight_->Initialize(dxEngine_);
}

void LightManager::Update()
{
	directionalLight_->Update();

	pointLight_->Update();

	spotLight_->Update();
}

void LightManager::Debug_ImGui()
{
#ifdef _DEBUG
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Light", flag)) {
		if (ImGui::TreeNodeEx("Directional Light", flag)) {
			directionalLight_->Debug_ImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Point Light", flag)) {
			pointLight_->Debug_ImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Spot Light", flag)) {
			spotLight_->Debug_ImGui();
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
#endif // _DEBUG
}

void LightManager::Finalize()
{
	delete instance_;
	instance_ = nullptr;
}
