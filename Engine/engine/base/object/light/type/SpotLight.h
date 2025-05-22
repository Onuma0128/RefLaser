#pragma once

#include "BaseLight.h"

#include "Vector3.h"
#include "Vector4.h"

class SpotLight : public BaseLight
{
public:

	struct SpotLightData {
		Vector4 color;
		Vector3 position;
		float intensity;
		Vector3 direction;
		float distance;
		float decay;
		float cosAngle;
		float cosFalloffStart;
		float padding;
	};

	void Initialize(DirectXEngine* dxEngine) override;

	void Update() override;

	void Debug_ImGui() override;

	void MakeLightData() override;


private:

	SpotLightData* lightData_ = nullptr;

};