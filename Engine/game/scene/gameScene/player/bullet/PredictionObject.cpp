#include "PredictionObject.h"

#include "gameScene/player/Player.h"

void PredictionObject::Init(const Vector3& offset)
{
	Object3d::Initialize("Box.obj");
	Object3d::SetSceneRenderer();
	transform_.scale_ = { 0.1f,0.1f,0.1f };

	offset_ = offset;
}

void PredictionObject::Update()
{
	Matrix4x4 rotateMat = Quaternion::MakeRotateMatrix(player_->GetRightStickQua());
	Vector3 position = offset_.Transform(rotateMat);
	transform_.translation_ = player_->GetTransform().translation_ + position;

	Object3d::Update();
}
