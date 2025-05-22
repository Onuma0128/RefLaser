#include "LaserCollider.h"

#include <algorithm>

#include "DeltaTimer.h"

void LaserCollider::Init()
{
	Collider::AddCollider();
	Collider::colliderName_ = "LaserCollider";
	Collider::myType_ = ColliderType::OBB;
	Collider::DrawCollider();

	activeFrame_ = 1.0f;
}

void LaserCollider::Update(std::vector<Vector3> positions)
{
	ActiveCollider();

	// レーザーに合わせたコライダーのTransformを初期化
	float size = (positions[0] - positions[1]).Length() / 2.0f;
	Vector3 velocity = (positions[0] - positions[1]);
	if (velocity.Length() != 0.0f) { velocity = velocity.Normalize(); }
	else { velocity = Vector3::ExprUnitZ; }
	Vector3 position = (positions[0] + positions[1]) * 0.5f;

	// レーザーに合わせたコライダーを設定する
	Collider::size_ = { activeFrame_,activeFrame_,size };
	Collider::rotate_ = VelocityToQuaternion(velocity, 1.0f);
	Collider::centerPosition_ = position;

	Collider::Update();
}

void LaserCollider::ActiveCollider()
{
	if (Collider::isActive_) {
		activeFrame_ -= DeltaTimer::GetDeltaTime();
		activeFrame_ = std::clamp(activeFrame_, 0.0f, 1.0f);
	} else {
		activeFrame_ = 1.0f;
	}
}

Quaternion LaserCollider::VelocityToQuaternion(const Vector3& velocity, const float lerp)
{
	Vector3 foward = Vector3::ExprUnitZ;
	Vector3 targetDir = Vector3{ -velocity.x,0.0f,velocity.z };

	// velocityから回転を求める
	Matrix4x4 targetMatrix = Matrix4x4::DirectionToDirection(foward, targetDir);
	Quaternion targetRotation = Quaternion::FormRotationMatrix(targetMatrix);
	
	return targetRotation;
}