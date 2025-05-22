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

	void PlayerMove();
	void LaserMove();
	void LaserAttack();

	Quaternion VelocityToQuaternion(const Vector3& velocity, const float lerp);

private:

	// 最後に向けたRightStickを保存する
	Vector3 rightStickVelocity_{};
	Quaternion rightStickQuaternion_{};
	// レーザーが何反射しているか
	uint32_t laserReflectCount_ = 0;

};

