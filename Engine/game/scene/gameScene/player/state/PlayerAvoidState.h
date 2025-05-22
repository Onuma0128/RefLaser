#pragma once

#include "PlayerBaseState.h"

#include "Quaternion.h"

class PlayerAvoidState :public PlayerBaseState
{
public:

	PlayerAvoidState(Player* player);

	void Init()override;

	void Finalize()override;

	void Update()override;

	void Draw()override;

private:

	float avoidTime_ = 0.0f;

	float velocityY_ = 0.0f;
	float acceleration_ = 0.0f;
	Quaternion rotateY_ = Quaternion::IdentityQuaternion();

};

