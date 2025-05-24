#include "Player.h"

#include "Input.h"
#include "Collision3D.h"

#include "state/PlayerMoveState.h"

void Player::Init()
{
	Object3d::Initialize("player_bottom.obj");
	Object3d::SetSceneRenderer();

	transform_.scale_ = { 0.5f,0.5f,0.5f };
	transform_.translation_ = { 0.0f,0.5f,0.0f };

	ChengeState(std::make_unique<PlayerMoveState>(this));

	effect_ = std::make_unique<PlayerEffect>();
	effect_->SetPlayer(this);
	effect_->Init();

	Collider::AddCollider();
	Collider::colliderName_ = "Player";
	Collider::myType_ = ColliderType::OBB;
	Collider::size_ = transform_.scale_;
	Collider::DrawCollider();

	playerTop_ = std::make_unique<PlayerTop>();
	playerTop_->Init();

	for (auto& laser : lasers_) {
		laser = std::make_unique<PlayerLaser>();
		laser->SetPlayer(this);
		laser->Init();
	}

	for (size_t i = 0; i < energys_.size(); ++i) {
		energys_[i] = std::make_unique<PlayerEnergy>();
		Quaternion q = Quaternion::MakeRotateAxisAngleQuaternion(
			Vector3::ExprUnitY, static_cast<float>(i) / static_cast<float>(energys_.size()) * 6.28f);
		Vector3 offset = (Vector3::ExprUnitZ * 3.0f).Transform(Quaternion::MakeRotateMatrix(q));

		energys_[i]->Init(transform_, offset);
	}
}

void Player::GlobalInit()
{

}

void Player::Update()
{
	state_->Update();

	effect_->Update();

	playerTop_->GetTransform().translation_ = transform_.translation_;
	playerTop_->Update(topQuaternion_);

	for (auto& energy : energys_) {
		energy->Update();
	}

	Collider::rotate_ = transform_.rotation_;
	Collider::centerPosition_ = transform_.translation_;
	Collider::Update();
	Object3d::Update();
}

void Player::ChengeState(std::unique_ptr<PlayerBaseState> newState)
{
	if (state_ != nullptr) {
		state_->Finalize();
	}
	state_ = std::move(newState);
	state_->Init();
}

void Player::OnCollisionEnter(Collider* other)
{
}

void Player::OnCollisionStay(Collider* other)
{
	if (other->GetColliderName() == "Wall") {
		Vector3 n; float d;
		if (Collision3D::OBBOBB_MTV(this, other, &n, &d))
		{
			Vector3 mtv = n * (d + 0.001f);

			// -------------------- 追加ここから --------------------
			Vector3 mtvXZ = { mtv.x, 0.0f, mtv.z };

			// 縦押しなら：中心差ベクトルの水平成分で代用
			if (mtvXZ.Length() < 1e-6f) {
				Vector3 centerDiff = other->GetCenterPosition() - GetCenterPosition();
				mtvXZ = centerDiff;
				mtvXZ.y = 0.0f;
				if (mtvXZ.Length() != 0.0f) {
					mtvXZ = mtvXZ.Normalize() * (d + 0.00001f);
				} else {
					mtvXZ = Vector3::ExprUnitZ * (d + 0.00001f);
				}
			}
			// -------------------- 追加ここまで --------------------

			transform_.translation_ -= mtvXZ;

			// スライド処理
			Vector3 slideN = mtvXZ.Normalize();
			float vn = Vector3::Dot(velocity_, slideN);
			if (vn < 0) velocity_ -= slideN * vn;

			// Collider を即更新して次ペアへの二次衝突を防ぐ
			centerPosition_ = transform_.translation_;
			Collider::Update();
			isHit_ = true;
		}
	}
}

void Player::OnCollisionExit(Collider* other)
{
	if (other->GetColliderName() == "Wall") {
		isHit_ = false;
	}
}
