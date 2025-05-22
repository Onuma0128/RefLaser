#include "Collision3D.h"

#include <algorithm>

#include "Collider.h"

bool Collision3D::SphereSphere(const Collider* a, const Collider* b)
{
	Sphere sphere1 = ChangeSphere(a);
	Sphere sphere2 = ChangeSphere(b);

	float distance = (sphere1.center - sphere2.center).Length();
	if (distance <= sphere1.radius + sphere2.radius) {
		return true;
	} else {
		return false;
	}
}

bool Collision3D::AABBSphere(const AABB aabb, const Sphere sphere)
{
	//最近接点を求める
	Vector3 closestPoint{
		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z)
	};
	//最近接点と球の中心との距離を求める
	float distance = (closestPoint - sphere.center).Length();
	//距離が半径よりも小さければ衝突
	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}

bool Collision3D::AABBSegment(const AABB aabb, const Segment segment)
{
	float tXmin = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float tXmax = (aabb.max.x - segment.origin.x) / segment.diff.x;
	float tYmin = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tYmax = (aabb.max.y - segment.origin.y) / segment.diff.y;
	float tZmin = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tZmax = (aabb.max.z - segment.origin.z) / segment.diff.z;

	float tNearX = std::min(tXmin, tXmax);
	float tFarX = std::max(tXmin, tXmax);
	float tNearY = std::min(tYmin, tYmax);
	float tFarY = std::max(tYmin, tYmax);
	float tNearZ = std::min(tZmin, tZmax);
	float tFarZ = std::max(tZmin, tZmax);
	//AABBとの衝突点のtが小さい方
	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	//AABBとの衝突点のtが大きい方
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tmin <= tmax && tmin <= 1.0f && tmax >= 0.0f) {
		return true;
	}
	return false;
}

bool Collision3D::OBBSphere(const Collider* a, const Collider* b)
{
	OBB obb = ChangeOBB(a);
	Sphere sphere = ChangeSphere(b);

	// OBBのWorld行列を作成
	Matrix4x4 worldInverse = Matrix4x4::Identity();

	for (int32_t i = 0; i < 3; ++i) {
		worldInverse.m[i][0] = obb.rotateMatrix.m[i][0];
		worldInverse.m[i][1] = obb.rotateMatrix.m[i][1];
		worldInverse.m[i][2] = obb.rotateMatrix.m[i][2];
		worldInverse.m[i][3] = 0.0f;
	}

	worldInverse.m[3][0] = obb.center.x;
	worldInverse.m[3][1] = obb.center.y;
	worldInverse.m[3][2] = obb.center.z;
	worldInverse.m[3][3] = 1.0f;

	// SphereをOBBローカル空間に入れる
	Vector3 centerInOBBLocalSpace = Vector3::Transform(sphere.center, Matrix4x4::Inverse(worldInverse));

	// OBBローカルのAABBとOBBローカルのSphereを作成
	AABB aabb_OBBLocal = {
		.min = -obb.size,
		.max = obb.size
	};
	Sphere sphere_OBBLocal = {
		.center = centerInOBBLocalSpace,
		.radius = sphere.radius
	};

	// AABBとSphereで判定を取る
	if (AABBSphere(aabb_OBBLocal, sphere_OBBLocal)) {
		return true;
	} else {
		return false;
	}
}

bool Collision3D::OBBSegment(const Collider* a, const Collider* b)
{
	OBB obb = ChangeOBB(a);
	Segment segment = ChangeSegment(b);

	// OBBのWorld行列を作成
	Matrix4x4 worldInverse = Matrix4x4::Identity();

	for (int32_t i = 0; i < 3; ++i) {
		worldInverse.m[i][0] = obb.rotateMatrix.m[i][0];
		worldInverse.m[i][1] = obb.rotateMatrix.m[i][1];
		worldInverse.m[i][2] = obb.rotateMatrix.m[i][2];
		worldInverse.m[i][3] = 0.0f;
	}

	worldInverse.m[3][0] = obb.center.x;
	worldInverse.m[3][1] = obb.center.y;
	worldInverse.m[3][2] = obb.center.z;
	worldInverse.m[3][3] = 1.0f;

	// SegmentをOBBローカル空間に入れる
	Vector3 originInOBBLocalSpace = Vector3::Transform(segment.origin, Matrix4x4::Inverse(worldInverse));
	// TransformNormaleにする
	Vector3 diffInOBBLocalSpace = Vector3::TransformNormal(segment.diff, Matrix4x4::Inverse(worldInverse));

	// OBBローカルのAABBとOBBローカルのSegmentを作成
	AABB aabb_OBBLocal = {
		.min = -obb.size,
		.max = obb.size
	};
	Segment segment_OBBLocal = {
		.origin = originInOBBLocalSpace,
		.diff = diffInOBBLocalSpace
	};

	// AABBとSegmentで判定を取る
	return AABBSegment(aabb_OBBLocal, segment_OBBLocal);
}

bool Collision3D::OBBOBB(const Collider* a, const Collider* b)
{
	// ① OBB 情報取得
	OBB obb1 = ChangeOBB(a);
	OBB obb2 = ChangeOBB(b);

	const Vector3 e1 = obb1.size;   // half-extent
	const Vector3 e2 = obb2.size;

	// ② 各 OBB のローカル軸（列ベクトルを抽出）
	Vector3 u1[3], u2[3];
	for (int i = 0; i < 3; ++i) {
		u1[i] = { obb1.rotateMatrix.m[0][i], obb1.rotateMatrix.m[1][i], obb1.rotateMatrix.m[2][i] };
		u2[i] = { obb2.rotateMatrix.m[0][i], obb2.rotateMatrix.m[1][i], obb2.rotateMatrix.m[2][i] };
	}

	// ③ 回転行列 R と |R|
	float R[3][3], AbsR[3][3];
	constexpr float kEps = 1e-5f;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			R[i][j] = Vector3::Dot(u1[i], u2[j]);
			AbsR[i][j] = std::abs(R[i][j]) + kEps;
		}
	}

	// ④ 中心間ベクトルを u1 基底に変換
	const Vector3 tWorld = obb2.center - obb1.center;
	const Vector3 Tvec = {
		Vector3::Dot(tWorld, u1[0]),
		Vector3::Dot(tWorld, u1[1]),
		Vector3::Dot(tWorld, u1[2])
	};

	// ⑤ 分離軸テスト
	// (A の 3 軸)
	for (int i = 0; i < 3; ++i) {
		const float ra = Vector3::AxisComponent(e1, i);
		const float rb =
			e2.x * AbsR[i][0] + e2.y * AbsR[i][1] + e2.z * AbsR[i][2];
		if (std::abs(Vector3::AxisComponent(Tvec, i)) > ra + rb) { return false; }
	}

	// (B の 3 軸)
	for (int j = 0; j < 3; ++j) {
		const float ra =
			e1.x * AbsR[0][j] + e1.y * AbsR[1][j] + e1.z * AbsR[2][j];
		const float rb = Vector3::AxisComponent(e2, j);
		const float proj = std::abs(Vector3::Dot(tWorld, u2[j]));
		if (proj > ra + rb) { return false; }
	}

	// (交差軸 u1[i] × u2[j] 9 本)
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			const int i1 = (i + 1) % 3, i2 = (i + 2) % 3;
			const int j1 = (j + 1) % 3, j2 = (j + 2) % 3;

			const float ra =
				Vector3::AxisComponent(e1, i1) * AbsR[i2][j] +
				Vector3::AxisComponent(e1, i2) * AbsR[i1][j];
			const float rb =
				Vector3::AxisComponent(e2, j1) * AbsR[i][j2] +
				Vector3::AxisComponent(e2, j2) * AbsR[i][j1];

			const float Tij = std::abs(
				Vector3::AxisComponent(Tvec, i2) * R[i1][j] -
				Vector3::AxisComponent(Tvec, i1) * R[i2][j]);

			if (Tij > ra + rb) { return false; }
		}
	}

	// どの軸でも分離できなければ衝突
	return true;
}

Sphere Collision3D::ChangeSphere(const Collider* collider)
{
	return {
		.center = collider->GetCenterPosition(),
		.radius = collider->GetRadius()
	};
}

Segment Collision3D::ChangeSegment(const Collider* collider)
{
	return  {
		.origin = collider->GetOrigin(),
		.diff = collider->GetDiff()
	};
}

AABB Collision3D::ChangeAABB(const Collider* collider)
{
	return {
		.min = collider->GetCenterPosition() - (collider->GetSize() * 0.5f),
		.max = collider->GetCenterPosition() + (collider->GetSize() * 0.5f)
	};
}

OBB Collision3D::ChangeOBB(const Collider* collider)
{
	return {
		.center = collider->GetCenterPosition(),
		.rotateMatrix = Quaternion::MakeRotateMatrix(collider->GetRotate()),
		.size = collider->GetSize()
	};
}
