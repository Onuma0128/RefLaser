#include "PlayerMoveState.h"

#include "Input.h"
#include "DeltaTimer.h"

#include "gameScene/player/Player.h"
#include "PlayerAvoidState.h"
#include "PlayerSpecialMoveState.h"

PlayerMoveState::PlayerMoveState(Player* player) :PlayerBaseState(player) {}

void PlayerMoveState::Init()
{
	rightStickVelocity_ = { 0.0f,0.0f,1.0f };
	rightStickQuaternion_ = Quaternion::IdentityQuaternion();
}

void PlayerMoveState::Finalize()
{
}

void PlayerMoveState::Update()
{
	// 移動処理
	PlayerMove();

	// レーザーの方向移動処理
	LaserMove();

	// レーザー発射する処理
	LaserAttack();
}

void PlayerMoveState::Draw()
{
}

void PlayerMoveState::PlayerMove()
{
	// Inputを取得
	Input* input = Input::GetInstance();

	// 移動の処理
	const float playerSpeed = 3.0f;
	Vector3 velocity{};
	velocity.x = input->GetGamepadLeftStickX();
	velocity.z = input->GetGamepadLeftStickY();

	if(!player_->IsWallHit())
	player_->GetTransform().translation_ += velocity * DeltaTimer::GetDeltaTime() * playerSpeed;

	// 移動時の回転の処理
	if (velocity.Length() != 0.0f) {
		// 回転を適応
		player_->GetTransform().rotation_ = VelocityToQuaternion(velocity, 0.1f);
		player_->SetVelocity(velocity);
	}
}

void PlayerMoveState::LaserMove()
{
	// 右のスティックのvelocityを取得
	Input* input = Input::GetInstance();
	Vector3 velocity{};
	velocity.x = input->GetGamepadRightStickX();
	velocity.z = input->GetGamepadRightStickY();
	// 押されているならvelocityを更新
	if (velocity.Length() != 0.0f) {
		velocity = velocity.Normalize();
		rightStickVelocity_ = Vector3::Lerp(rightStickVelocity_, velocity, 0.1f);
		Quaternion target = VelocityToQuaternion(rightStickVelocity_, 1.0f);
		player_->SetTopQuaternion(Quaternion::Slerp(player_->GetTopQuaternion(), target, 0.5f));
	}

	Vector3 hitPos{};
	Vector3 hitDir{};
	bool isHit = false;
	bool start = false;
	laserReflectCount_ = 1;

	for (auto& laser : player_->GetLasers()) {
		if (!start) {
			laser->Update(player_->GetTransform().translation_, rightStickVelocity_);
			isHit = laser->GetIsHit();
			laser->SetIsHit();
			if (isHit) {
				hitPos = laser->GetHitPosition();
				hitDir = laser->GetHitDirection();
			} else {
				hitPos = player_->GetTransform().translation_;
				hitDir = rightStickVelocity_;
			}
			start = true;
		} else {
			if (isHit) {
				laser->Update(hitPos, hitDir);
				hitPos = laser->GetHitPosition();
				hitDir = laser->GetHitDirection();
				++laserReflectCount_;
			} else {
				hitPos = player_->GetTransform().translation_;
				hitDir = rightStickVelocity_;
				laser->Update(hitPos, hitDir);
			}
			isHit = laser->GetIsHit();
			laser->SetIsHit();
		}
	}
}

void PlayerMoveState::LaserAttack()
{
	// Inputを取得
	Input* input = Input::GetInstance();

	// ボタンが押されたら
	if (input->TriggerGamepadButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		// レーザーの反射している分だけコライダーをアクティブにする
		uint32_t count = 0;
		for (auto& laser : player_->GetLasers()) {
			if (laserReflectCount_ == count) { break; }
			laser->CreateLaser();
			laser->SetIsAttack(true);
			++count;
		}
		count = 0;
		for (auto& energy : player_->GetEnergys()) {
			if (laserReflectCount_ == count) { break; }
			if (!energy->GetActive()) { continue; }
			energy->SetActive(false);
			++count;
		}
	}
}

Quaternion PlayerMoveState::VelocityToQuaternion(const Vector3& velocity, const float lerp)
{
	Vector3 foward = Vector3::ExprUnitZ;
	Vector3 targetDir = Vector3{ -velocity.x,0.0f,velocity.z };

	// velocityから回転を求める
	Matrix4x4 targetMatrix = Matrix4x4::DirectionToDirection(foward, targetDir);
	Quaternion targetRotation = Quaternion::FormRotationMatrix(targetMatrix);
	Quaternion currentRotation = player_->GetTransform().rotation_;
	Quaternion result = Quaternion::Slerp(currentRotation, targetRotation, lerp);

	return result;
}
