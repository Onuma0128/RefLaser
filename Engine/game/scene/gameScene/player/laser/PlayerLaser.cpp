#include "PlayerLaser.h"

#include <algorithm>

#include "Collision3D.h"
#include "DeltaTimer.h"

#include "gameScene/player/Player.h"

void PlayerLaser::Init()
{
	laserLine_ = std::make_unique<Line3d>();
	linePositions_.resize(2);
	for (auto& position : linePositions_) {
		position = Vector3{};
	}
	laserLine_->Initialize(linePositions_);


	Collider::AddCollider();
	Collider::myType_ = ColliderType::Segment;
	Collider::colliderName_ = "LaserLine";

	collider_ = std::make_unique<LaserCollider>();
	collider_->Init();

	attackFrame_ = 1.0f;
}

void PlayerLaser::Update(const Vector3& start, const Vector3& end)
{
	hitPointLength_ = 1000.0f;

	if (isHit_) {
		linePositions_ = { start,hitPosition_ };
	} else {
		linePositions_ = { start,(end * 100.0f) + start };
	}

	startPosition_ = start;
	endVelocity_ = end;
	laserLine_->SetPositions(linePositions_);
	laserLine_->Update();

	Collider::origin_ = start;
	Collider::diff_ = end * 100.0f;
	Collider::Update();

	AttackFrame();
	collider_->SetActive(isAttack_);
	collider_->Update(linePositions_);
}

void PlayerLaser::OnCollisionEnter(Collider* other)
{
}

void PlayerLaser::OnCollisionStay(Collider* other)
{
	if (other->GetColliderName() == "Wall") {
		RaycastHit hit{};
		if (Collision3D::OBBSegment(other, this, &hit))
		{
			float length = (hit.point - startPosition_).Length();
			if (hitPointLength_ < length) { return; }
			hitPointLength_ = length;
			hitDirection_ = Vector3::Reflect(Collider::diff_.Normalize(), hit.normal).Normalize();
			hitPosition_ = hit.point + (endVelocity_ * 0.05f);
			isHit_ = true;
		}
	}
}

void PlayerLaser::OnCollisionExit(Collider* other)
{
	if (other->GetColliderName() == "Wall") {
		isHit_ = false;
	}
}

void PlayerLaser::AttackFrame()
{
	attackFrame_ -= DeltaTimer::GetDeltaTime();
	attackFrame_ = std::clamp(attackFrame_, 0.0f, 1.0f);


	if (attackFrame_ <= 0.0f && isAttack_) {
		isAttack_ = false;
		attackFrame_ = 1.0f;
	}
}
