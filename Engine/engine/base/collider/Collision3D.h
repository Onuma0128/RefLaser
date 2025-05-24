#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

// 球
struct Sphere {
	Vector3 center;
	float radius;
};
// 線
struct Segment {
	Vector3 origin;
	Vector3 diff;
};
struct RaycastHit {
	Vector3 point; 
	Vector3 normal;
	float   t;     
};
// 回転無しボックス
struct AABB {
	Vector3 min;
	Vector3 max;
};
// 回転有りボックス
struct OBB {
	Vector3 center;
	Matrix4x4 rotateMatrix;
	Vector3 size;
};

class Collider;

class Collision3D
{
public:

	// 球と球の衝突判定
	static bool SphereSphere(const Collider* a, const Collider* b);

	// AABBと球の衝突判定
	static bool AABBSphere(const AABB aabb, const Sphere sphere);

	// AABBとSegmentの衝突判定
	static bool AABBSegment(const AABB aabb, const Segment segment);

	// OBBと球の衝突判定 
	static bool OBBSphere(const Collider* a, const Collider* b);

	// OBBとSegmentの衝突判定
	static bool OBBSegment(const Collider* a, const Collider* b);
	static bool OBBSegment(const Collider* obbCol, const Collider* segCol, RaycastHit* hit);

	// OBBとOBBの衝突判定
	static bool OBBOBB(const Collider* a, const Collider* b);

	static bool OBBOBB_MTV(const Collider* colA, const Collider* colB, Vector3* n, float* depth);

private:

	static Sphere ChangeSphere(const Collider* collider);
	static Segment ChangeSegment(const Collider* collider);
	static AABB ChangeAABB(const Collider* collider);
	static OBB ChangeOBB(const Collider* collider);
};
