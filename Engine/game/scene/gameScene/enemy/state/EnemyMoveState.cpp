#include "EnemyMoveState.h"

#include "DeltaTimer.h"

#include "gameScene/enemy/enemy.h"
#include "gameScene/player/Player.h"

EnemyMoveState::EnemyMoveState(Enemy* enemy) :EnemyBaseState(enemy) {}

void EnemyMoveState::Init()
{
}

void EnemyMoveState::Finalize()
{
}

void EnemyMoveState::Update()
{
	// 移動処理
	const float speed = 1.0f;
	Vector3 velocity = 
		Vector3(enemy_->GetPlayer()->GetTransform().translation_ - enemy_->GetTransform().translation_);
	velocity.y = 0.0f;
	if (velocity.Length() != 0.0f) { velocity = velocity.Normalize(); }
	enemy_->GetTransform().translation_ += velocity * speed * DeltaTimer::GetDeltaTime();
	enemy_->SetVelocity(velocity);

	// 移動時の回転の処理
	if (velocity.Length() != 0.0f) {
		Vector3 foward = Vector3::ExprUnitZ;
		Vector3 targetDir = Vector3{ -velocity.x,0.0f,velocity.z };

		// velocityから回転を求める
		Matrix4x4 targetMatrix = Matrix4x4::DirectionToDirection(foward, targetDir);
		Quaternion targetRotation = Quaternion::FormRotationMatrix(targetMatrix);
		Quaternion currentRotation = enemy_->GetTransform().rotation_;
		Quaternion result = Quaternion::Slerp(currentRotation, targetRotation, 0.1f);

		// 回転を適応
		enemy_->GetTransform().rotation_ = result;
	}
}

void EnemyMoveState::Draw()
{
}
