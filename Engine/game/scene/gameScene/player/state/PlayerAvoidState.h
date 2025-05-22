#pragma once

#include "PlayerBaseState.h"

class PlayerAvoidState :public PlayerBaseState
{
public:

	PlayerAvoidState(Player* player);

	void Init()override;

	void Finalize()override;

	void Update()override;

	void Draw()override;

private:

};

