#include "PointLight.h"

#include "DirectXEngine.h"
#include "imgui.h"

#include "CreateBufferResource.h"

void PointLight::Initialize(DirectXEngine* dxEngine)
{
	dxEngine_ = dxEngine;
	MakeLightData();
}

void PointLight::Update()
{
}

void PointLight::Debug_ImGui()
{
	ImGui::ColorEdit4("LightColor", (float*)&lightData_->color.x);
	ImGui::DragFloat3("PointLightData.pos", &lightData_->position.x, 0.01f);
	ImGui::DragFloat("PointLightIntensity", &lightData_->intensity, 0.01f);
	ImGui::DragFloat("PointLightRadius", &lightData_->radius, 0.01f);
	ImGui::DragFloat("PointLightDecay", &lightData_->decay, 0.01f);
}

void PointLight::MakeLightData()
{
	resource_ = CreateBufferResource(dxEngine_->GetDevice(), sizeof(PointLightData)).Get();
	bufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
	bufferView_.SizeInBytes = sizeof(PointLightData);
	bufferView_.StrideInBytes = sizeof(PointLightData);
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));
	// デフォルト値
	lightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	lightData_->position = { 0.0f,2.0f,2.0f };
	lightData_->intensity = 0.0f;
	lightData_->radius = 6.0f;
	lightData_->decay = 2.0f;
}
