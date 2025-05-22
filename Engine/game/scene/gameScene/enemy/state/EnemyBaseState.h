#pragma once

class Enemy;

class EnemyBaseState
{
public:

	~EnemyBaseState() = default;
	EnemyBaseState(Enemy* enemy);

	virtual void Init() = 0;

	virtual void Finalize() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

protected:

	Enemy* enemy_ = nullptr;
};
