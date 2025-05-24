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

bool Collision3D::OBBSegment(const Collider* obbCol, const Collider* segCol, RaycastHit* hit)
{
	// ① 既存関数と同じ前処理
	OBB      obb = ChangeOBB(obbCol);
	Segment  segW = ChangeSegment(segCol);

	// OBB ローカル空間へ変換（既存処理をそのまま使用）
	Matrix4x4 invM = Matrix4x4::Inverse(
		Matrix4x4::Translate(obb.center) * obb.rotateMatrix);

	Segment segL = {
		.origin = Vector3::Transform(segW.origin,invM),
		.diff = Vector3::TransformNormal(segW.diff,invM)
	};

	// ② AABB×Segment 判定（ローカル空間）— 元コードをほぼコピペ
	float tNear = 0.f, tFar = 1.f;
	int   hitAxis = -1;               // まだ決まっていないことを示す
	int   hitSign = 0;
	const Vector3 min = -obb.size, max = obb.size;

	auto Slab = [&](float segO, float segD, float slabMin, float slabMax, int axis)
		{
			if (std::abs(segD) < 1e-6f) {        // 平行
				return (segO < slabMin || segO > slabMax);
			}
			float invD = 1.0f / segD;
			float t1 = (slabMin - segO) * invD;
			float t2 = (slabMax - segO) * invD;
			if (t1 > t2) std::swap(t1, t2);

			// Near で入った面の法線符号：  +diff ⇒ -面， -diff ⇒ +面
			int signNear = (segD > 0.0f) ? -1 : 1;
			if (t1 > tNear) { tNear = t1; hitAxis = axis; hitSign = signNear; }
			if (t2 < tFar) { tFar = t2; }
			return (tNear > tFar || tFar < 0.f || tNear > 1.f);
		};
	if (Slab(segL.origin.x, segL.diff.x, min.x, max.x, 0) ||
		Slab(segL.origin.y, segL.diff.y, min.y, max.y, 1) ||
		Slab(segL.origin.z, segL.diff.z, min.z, max.z, 2))
	{
		return false;
	}

	// ③ ここまで来れば衝突。追加情報が欲しい場合だけ計算
	if (hit) {
		// 衝突点（ローカル → ワールド）
		Vector3 pL = segL.origin + segL.diff * tNear;
		hit->point = Vector3::Transform(pL, obb.rotateMatrix) + obb.center;

		// 法線（ローカル ±X/Y/Z → ワールド）
		if (hitAxis < 0) { return false; }

		Vector3 nL{};
		nL[hitAxis] = static_cast<float>(hitSign);
		hit->normal = Vector3::TransformNormal(nL, obb.rotateMatrix).Normalize();

		// t 値
		hit->t = tNear;
	}
	return true;
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

bool Collision3D::OBBOBB_MTV(const Collider* colA, const Collider* colB,
	Vector3* n, float* depth)
{
	constexpr float kEpsilon = 1e-6f;

	// ---------- 1) 変数展開 -------------------------------------------------
	OBB A = ChangeOBB(colA);
	OBB B = ChangeOBB(colB);

	// A のローカル基底ベクトル
	auto Aaxis = [&](int i) -> Vector3 {
		return { A.rotateMatrix.m[0][i], A.rotateMatrix.m[1][i], A.rotateMatrix.m[2][i] };
		};
	// B のローカル基底ベクトル
	auto Baxis = [&](int i) -> Vector3 {
		return { B.rotateMatrix.m[0][i], B.rotateMatrix.m[1][i], B.rotateMatrix.m[2][i] };
		};

	// 世界→A 空間への変換は R^T;  ここではドット積で都度計算
	Vector3 tWorld = B.center - A.center;
	Vector3 t = { Vector3::Dot(tWorld, Aaxis(0)),
				  Vector3::Dot(tWorld, Aaxis(1)),
				  Vector3::Dot(tWorld, Aaxis(2)) };

	// R_AB[i][j] = Ai・Bj
	float R[3][3], AbsR[3][3];
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j) {
			R[i][j] = Vector3::Dot(Aaxis(i), Baxis(j));
			AbsR[i][j] = std::fabs(R[i][j]) + kEpsilon;   // 浮動小数点誤差ガード
		}

	const Vector3 aExt = A.size;
	const Vector3 bExt = B.size;

	// ---------- 2) SAT テスト 15 軸 -----------------------------------------
	float minOverlap = FLT_MAX;
	Vector3 axisMin = Vector3::ExprUnitX;   // A 空間軸で保持
	bool    axisIsA = true;                 // True: A の軸, False: それ以外

	auto TestAxis = [&](const Vector3& L, float dist, float rA, float rB, bool fromA)->bool
		{
			float overlap = rA + rB - dist;
			if (overlap < 0.f) return false;         // 分離軸発見 → 非衝突

			if (overlap < minOverlap) {
				minOverlap = overlap;
				axisMin = L;
				axisIsA = fromA;
			}
			return true;
		};

	// --- (A の 3 軸)
	for (int i = 0; i < 3; ++i) {
		float dist = std::fabs(t[i]);
		float rA = aExt[i];
		float rB = bExt.x * AbsR[i][0] + bExt.y * AbsR[i][1] + bExt.z * AbsR[i][2];
		if (!TestAxis(Vector3::ExprUnitX * (i == 0) + Vector3::ExprUnitY * (i == 1) + Vector3::ExprUnitZ * (i == 2),
			dist, rA, rB, true)) return false;
	}

	// --- (B の 3 軸)
	for (int j = 0; j < 3; ++j) {
		Vector3 L = { R[0][j], R[1][j], R[2][j] };          // A 空間での Bj
		float dist = std::fabs(t.x * R[0][j] + t.y * R[1][j] + t.z * R[2][j]);
		float rA = aExt.x * AbsR[0][j] + aExt.y * AbsR[1][j] + aExt.z * AbsR[2][j];
		float rB = bExt[j];
		if (!TestAxis(L, dist, rA, rB, false)) return false;
	}

	// --- (交叉軸 9 本  Ai×Bj)
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
		{
			// 外積の大きさが 0 ⇒ 平行軸 ⇒ スキップ
			Vector3 L = Vector3::Cross(Aaxis(i), Baxis(j));
			if (L.Length() < kEpsilon) continue;

			// 投影距離＝ |t・(Ai×Bj)|
			float dist = std::fabs(
				t[(i + 1) % 3] * R[(i + 2) % 3][j] - t[(i + 2) % 3] * R[(i + 1) % 3][j]);

			// 投影半径（Real-Time Collision Detection 式）
			float rA = aExt[(i + 1) % 3] * AbsR[(i + 2) % 3][j] +
				aExt[(i + 2) % 3] * AbsR[(i + 1) % 3][j];

			float rB = bExt[(j + 1) % 3] * AbsR[i][(j + 2) % 3] +
				bExt[(j + 2) % 3] * AbsR[i][(j + 1) % 3];

			if (!TestAxis(L.Normalize(), dist, rA, rB, false)) return false;
		}

	// ---------- 3) MTV をワールド座標で返す ------------------------------
	if (n && depth) {
		// axisMin は A 空間。符号を A→B 方向に合わせてから世界へ戻す
		float sign = (Vector3::Dot(axisMin, t) < 0.f) ? -1.f : 1.f;
		Vector3 nA = axisMin * sign;
		Vector3 nW = {
			nA.x * A.rotateMatrix.m[0][0] + nA.y * A.rotateMatrix.m[0][1] + nA.z * A.rotateMatrix.m[0][2],
			nA.x * A.rotateMatrix.m[1][0] + nA.y * A.rotateMatrix.m[1][1] + nA.z * A.rotateMatrix.m[1][2],
			nA.x * A.rotateMatrix.m[2][0] + nA.y * A.rotateMatrix.m[2][1] + nA.z * A.rotateMatrix.m[2][2] };

		*n = nW.Normalize();   // 押し出し法線（長さ 1）
		*depth = minOverlap;       // めり込み深さ (>0)
	}
	return true;                   // 交差している
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
