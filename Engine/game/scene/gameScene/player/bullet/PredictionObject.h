#pragma once

#include "Object3d.h"

class Player;

class PredictionObject :public Object3d
{
public:

	void Init(const Vector3& offset);

	void Update();

	void SetPlayer(Player* player) { player_ = player; }

private:

	Player* player_ = nullptr;
	Vector3 offset_{};

};
