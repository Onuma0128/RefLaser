#pragma once

#include "BaseLight.h"

#include "Vector3.h"
#include "Vector4.h"

class DirectionalLight : public BaseLight
{
public:

	struct DirectionalLightData {
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

	void Initialize(DirectXEngine* dxEngine) override;

	void Update() override;

	void Debug_ImGui() override;

	void MakeLightData() override;


private:

	DirectionalLightData* lightData_ = nullptr;

};