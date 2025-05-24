#pragma once

#include <memory>
#include <vector>

#include "Line3d.h"
#include "PrimitiveDrawr.h"
#include "Collider.h"
#include "Vector3.h"

#include "LaserCollider.h"

class Player;

class PlayerLaser :public Collider
{
public:

	void Init();

	void Update(const Vector3& start, const Vector3& end);

	void OnCollisionEnter(Collider* other) override;
	void OnCollisionStay(Collider* other) override;
	void OnCollisionExit(Collider* other) override;

	void SetPlayer(Player* player) { player_ = player; }

	const Vector3& GetHitPosition() { return hitPosition_; }
	const Vector3& GetHitDirection() { return hitDirection_; }
	const bool GetIsHit() { return isHit_; }
	void SetIsHit(){ isHit_ = false; }

	void SetIsAttack(bool flag) { isAttack_ = flag; }
	void CreateLaser();

private:

	void AttackFrame();

private:

	Player* player_ = nullptr;

	// レーザーの描画
	std::unique_ptr<PrimitiveDrawr> laserPlane_ = nullptr;
	// レーザーのライン描画
	std::unique_ptr<Line3d> laserLine_ = nullptr;
	// レーザーのコライダー
	std::unique_ptr<LaserCollider> collider_ = nullptr;

	// ヒットした壁が近いかを判別する距離
	float hitPointLength_ = 1000.0f;
	// 更新で受け取ったラインの座標
	Vector3 startPosition_{};
	Vector3 endVelocity_{};
	std::vector<Vector3> linePositions_;

	// ヒットした時に出る反射した座標と反射ベクトル
	Vector3 hitPosition_;
	Vector3 hitDirection_;

	// 今攻撃しているか
	bool isAttack_ = false;
	float attackFrame_ = 0.0f;
	// 今ヒットしているか
	bool isHit_ = false;

};

