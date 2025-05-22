#include "PlayerEnergy.h"

#include "DeltaTimer.h"
#include "Easing.h"

void PlayerEnergy::Init(const WorldTransform& transform, const Vector3& offset)
{
	Object3d::Initialize("Box.obj");
	Object3d::SetSceneRenderer();

	transform_.parent_ = &transform;
	transform_.translation_ = offset;
	transform_.scale_ = { 0.4f,0.4f,0.4f };
}

void PlayerEnergy::Update()
{
	RevivalUpdate();

	scaleFrame_ += DeltaTimer::GetDeltaTime();
	scaleFrame_ = std::clamp(scaleFrame_, 0.0f, 1.0f);
	float t = Easing::EaseOutCirc(scaleFrame_);
	float scale = 0.4f + t * 0.2f;
	transform_.scale_ = { scale ,scale ,scale };

	if (scaleFrame_ >= 1.0f) { scaleFrame_ = 0.0f; }

	Object3d::Update();
}

void PlayerEnergy::RevivalUpdate()
{
	if (Object3d::GetRenderOptions().enabled) { return; }

	revivalFrame_ += DeltaTimer::GetDeltaTime();
	revivalFrame_ = std::clamp(revivalFrame_, 0.0f, 1.0f);

	if (revivalFrame_ >= 1.0f) { 
		revivalFrame_ = 0.0f;
		Object3d::GetRenderOptions().enabled = true;
	}
}
