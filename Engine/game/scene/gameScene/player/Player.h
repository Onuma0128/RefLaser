#pragma once
#include <memory>
#include <array>
#include <vector>

#include "Object3d.h"
#include "Collider.h"

#include "gameScene/player/state/PlayerBaseState.h"
#include "gameScene/player/effect/PlayerEffect.h"

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

	Quaternion GetRightStickQua()const { return rightStickQuaternion_; }
	void SetRightStickQua(const Quaternion& q) { rightStickQuaternion_ = q; }

	std::array<std::unique_ptr<PlayerLaser>, 3>& GetLasers() { return lasers_; }
	std::array<std::unique_ptr<PlayerEnergy>, 10>& GetEnergys() { return energys_; }

private:

	// 状態遷移
	std::unique_ptr<PlayerBaseState> state_ = nullptr;
	// エフェクト
	std::unique_ptr<PlayerEffect> effect_ = nullptr;

	// レーザーを撃つ方向の回転
	Quaternion rightStickQuaternion_{};

	// レーザー
	std::array<std::unique_ptr<PlayerLaser>, 3> lasers_;

	// エネルギー
	std::array<std::unique_ptr<PlayerEnergy>, 10> energys_;

};