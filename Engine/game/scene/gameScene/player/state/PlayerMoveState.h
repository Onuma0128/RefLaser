#pragma once

#include "PlayerBaseState.h"

#include "Vector3.h"
#include "Quaternion.h"

class PlayerMoveState : public PlayerBaseState
{
public:

	PlayerMoveState(Player* player);

	void Init()override;

	void Finalize()override;

	void Update()override;

	void Draw()override;

private:

	void ReloadBullet();

	Quaternion VelocityToQuaternion(const Vector3& velocity, const float lerp);

private:

	// 今弾をリロードしているか
	bool isReloadBullet_ = false;
	// 一つの弾をリロードする時間
	float reloadBulletTime_;

	// 最後に向けたRightStickを保存する
	Vector3 rightStickVelocity_{};
	Quaternion rightStickQuaternion_{};

};

