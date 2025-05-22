#pragma once

class Player;

class PlayerBaseState
{
public:

	~PlayerBaseState() = default;
	PlayerBaseState(Player* player);

	virtual void Init() = 0;

	virtual void Finalize() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

protected:

	Player* player_ = nullptr;

};