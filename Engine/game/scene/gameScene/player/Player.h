#pragma once
#include <memory>
#include <array>
#include <vector>

#include "Object3d.h"
#include "Collider.h"

#include "gameScene/player/state/PlayerBaseState.h"
#include "gameScene/player/effect/PlayerEffect.h"

#include "gameScene/player/PlayerTop.h"
#include "gameScene/player/laser/PlayerLaser.h"
#include "gameScene/player/energy/PlayerEnergy.h"

class Player : public Object3d,Collider
{
public:

	void Init();

	void GlobalInit();

	void Update();

	void ChengeState(std::unique_ptr<PlayerBaseState> newState);

	void OnCollisionEnter(Collider* other) override;
	void OnCollisionStay(Collider* other) override;
	void OnCollisionExit(Collider* other) override;

	Vector3 GetVelocity()const { return velocity_; }
	void SetVelocity(const Vector3& v) { velocity_ = v; }

	Quaternion GetTopQuaternion()const { return topQuaternion_; }
	void SetTopQuaternion(const Quaternion& q) { topQuaternion_ = q; }

	bool IsWallHit()const { return isHit_; }

	std::array<std::unique_ptr<PlayerLaser>, 3>& GetLasers() { return lasers_; }
	std::array<std::unique_ptr<PlayerEnergy>, 10>& GetEnergys() { return energys_; }

private:

	// 状態遷移
	std::unique_ptr<PlayerBaseState> state_ = nullptr;
	// エフェクト
	std::unique_ptr<PlayerEffect> effect_ = nullptr;
	// プレイヤーの上部分
	std::unique_ptr<PlayerTop> playerTop_ = nullptr;
	Quaternion topQuaternion_ = Quaternion::IdentityQuaternion();

	// 速度
	Vector3 velocity_{};
	bool isHit_ = false;

	// レーザー
	std::array<std::unique_ptr<PlayerLaser>, 3> lasers_;
	// エネルギー
	std::array<std::unique_ptr<PlayerEnergy>, 10> energys_;

};