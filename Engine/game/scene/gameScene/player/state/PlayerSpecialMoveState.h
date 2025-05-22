#pragma once

#include "PlayerBaseState.h"

class PlayerSpecialMoveState : public PlayerBaseState
{
public:

	PlayerSpecialMoveState(Player* player);

	void Init()override;

	void Finalize()override;

	void Update()override;

	void Draw()override;

private:

};

