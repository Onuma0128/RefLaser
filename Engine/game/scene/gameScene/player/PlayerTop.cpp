#include "PlayerTop.h"

void PlayerTop::Init()
{
	Object3d::Initialize("player_top.obj");
	Object3d::SetSceneRenderer();
	transform_.scale_ = { 0.5f,0.5f ,0.5f };
}

void PlayerTop::Update(const Quaternion& q)
{
	transform_.rotation_ = q;

	Object3d::Update();
}
