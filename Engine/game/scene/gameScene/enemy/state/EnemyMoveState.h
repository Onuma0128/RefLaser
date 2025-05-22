#pragma once

#include "EnemyBaseState.h"

class EnemyMoveState : public EnemyBaseState
{
public:

	EnemyMoveState(Enemy* enemy);

	void Init()override;

	void Finalize()override;

	void Update()override;

	void Draw()override;
};

