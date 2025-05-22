#include "PlayerAvoidState.h"

#include <numbers>

#include "DeltaTimer.h"

#include "gameScene/player/Player.h"
#include "PlayerMoveState.h"

PlayerAvoidState::PlayerAvoidState(Player* player) :PlayerBaseState(player) {}

void PlayerAvoidState::Init()
{
	velocityY_ = 16.0f;
	acceleration_ = 2.0f;

	rotateY_ = Quaternion::ExtractYawQuaternion(player_->GetTransform().rotation_);
}

void PlayerAvoidState::Finalize()
{
}

void PlayerAvoidState::Update()
{
	const float speed = 20.0f;
	avoidTime_ += DeltaTimer::GetDeltaTime() * 4.0f;
	avoidTime_ = std::clamp(avoidTime_, 0.0f, 1.0f);

	Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(rotateY_);
	Vector3 velocity = Vector3::ExprUnitZ.Transform(rotateMatrix).Normalize();

	// 回転を適応
	Quaternion rotateX = Quaternion::MakeRotateAxisAngleQuaternion
	(Vector3::ExprUnitX, (avoidTime_ * 2.0f) * std::numbers::pi_v<float>);
	player_->GetTransform().rotation_ = rotateY_ * rotateX;
	// 座標を更新
	player_->GetTransform().translation_ += velocity * speed * DeltaTimer::GetDeltaTime();

	velocityY_ -= acceleration_;
	player_->GetTransform().translation_.y += velocityY_ * DeltaTimer::GetDeltaTime();

	if (avoidTime_ >= 1.0f) {
		player_->GetTransform().translation_.y = 0.5f;
		player_->ChengeState(std::make_unique<PlayerMoveState>(player_));
	}
}

void PlayerAvoidState::Draw()
{
}
