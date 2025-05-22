#pragma once

#include "BaseLight.h"

#include "Vector3.h"
#include "Vector4.h"

class PointLight : public BaseLight
{
public:

	struct PointLightData {
		Vector4 color;
		Vector3 position;
		float intensity;
		float radius;
		float decay;
		float padding[2];
	};

	void Initialize(DirectXEngine* dxEngine) override;

	void Update() override;

	void Debug_ImGui() override;

	void MakeLightData() override;


private:

	PointLightData* lightData_ = nullptr;

};

