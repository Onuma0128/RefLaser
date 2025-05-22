#include "DirectionalLight.h"

#include "DirectXEngine.h"
#include "imgui.h"

#include "CreateBufferResource.h"

void DirectionalLight::Initialize(DirectXEngine* dxEngine)
{
	dxEngine_ = dxEngine;
	MakeLightData();
}

void DirectionalLight::Update()
{
	lightData_->direction = lightData_->direction.Normalize();
}

void DirectionalLight::Debug_ImGui()
{
	ImGui::ColorEdit4("LightColor", (float*)&lightData_->color.x);
	ImGui::DragFloat3("DirectionalLightData.Direction", &lightData_->direction.x, 0.01f);
	ImGui::DragFloat("DirectionalLightData.intensity", &lightData_->intensity, 0.01f);
}

void DirectionalLight::MakeLightData()
{
	resource_ = CreateBufferResource(dxEngine_->GetDevice(), sizeof(DirectionalLightData)).Get();
	bufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
	bufferView_.SizeInBytes = sizeof(DirectionalLightData);
	bufferView_.StrideInBytes = sizeof(DirectionalLightData);
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));
	// デフォルト値
	lightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	lightData_->direction = { 0.0f,1.0f,0.0f };
	lightData_->intensity = 1.0f;
}
