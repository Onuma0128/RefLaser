#pragma once

#include "Object3d.h"

class PlayerEnergy : public Object3d
{
public:

	void Init(const WorldTransform& transform, const Vector3& offset);

	void Update();

	void SetActive(bool flag) { Object3d::GetRenderOptions().enabled = flag; }
	bool GetActive() { return Object3d::GetRenderOptions().enabled; }

private:

	void RevivalUpdate();

private:

	float scaleFrame_ = 0.0f;

	// エナジーが回復するフレーム
	float revivalFrame_ = 0.0f;

};