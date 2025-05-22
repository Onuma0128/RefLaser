#include "SpotLight.h"

#include <numbers>

#include "DirectXEngine.h"
#include "imgui.h"

#include "CreateBufferResource.h"

void SpotLight::Initialize(DirectXEngine* dxEngine)
{
	dxEngine_ = dxEngine;
	MakeLightData();
}

void SpotLight::Update()
{
	lightData_->direction = lightData_->direction.Normalize();
}

void SpotLight::Debug_ImGui()
{
	ImGui::ColorEdit4("LightColor", (float*)&lightData_->color.x);
	ImGui::DragFloat3("SpotLightData.pos", &lightData_->position.x, 0.01f);
	ImGui::DragFloat3("SpotLightData.direction", &lightData_->direction.x, 0.01f);
	ImGui::DragFloat("SpotLightData.intensity", &lightData_->intensity, 0.01f);
	ImGui::DragFloat("SpotLightData.distance", &lightData_->distance, 0.01f);
	ImGui::DragFloat("SpotLightData.decay", &lightData_->decay, 0.01f);
	ImGui::DragFloat("SpotLightData.cosAngle", &lightData_->cosAngle, 0.01f);
	ImGui::DragFloat("SpotLightData.cosFalloffStart", &lightData_->cosFalloffStart, 0.01f);
}

void SpotLight::MakeLightData()
{
	resource_ = CreateBufferResource(dxEngine_->GetDevice(), sizeof(SpotLightData)).Get();
	bufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
	bufferView_.SizeInBytes = sizeof(SpotLightData);
	bufferView_.StrideInBytes = sizeof(SpotLightData);
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));
	// デフォルト値
	lightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	lightData_->position = { -3.0f,0.0f,0.0f };
	lightData_->distance = 7.0f;
	lightData_->direction = (Vector3{ -1.0f,-1.0f,0.0f }.Normalize());
	lightData_->intensity = 0.0f;
	lightData_->decay = 2.0f;
	lightData_->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	lightData_->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 5.0f);
}
