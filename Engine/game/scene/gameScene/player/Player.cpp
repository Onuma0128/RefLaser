#include "Player.h"

#include "Input.h"

#include "state/PlayerMoveState.h"

void Player::Init()
{
	Object3d::Initialize("Box.obj");
	Object3d::GetRenderOptions().enabled = false;

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

	rightStickQuaternion_ = Quaternion::IdentityQuaternion();

	for (auto& laser : lasers_) {
		laser = std::make_unique<PlayerLaser>();
		laser->SetPlayer(this);
		laser->Init();
	}
}

void Player::GlobalInit()
{

}

void Player::Update()
{
	state_->Update();

	effect_->Update();

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
}

void Player::OnCollisionExit(Collider* other)
{
}
