#pragma once
#include <memory>
#include <list>

#include "gameScene/enemy/Enemy.h"
#include "EnemySpawner.h"

class EnemySpawnerFactory
{
public:

	void Init();

	void Update();

	void Draw();

	Player* GetPlayer() { return player_; }
	void SetPlayer(Player* player) { player_ = player; }

	GameCamera* GetGameCamera() { return gameCamera_; }
	void SetGameCamera(GameCamera* camera) { gameCamera_ = camera; }

	void CreateSpawner(const Vector3& position);

private:

	// ゲッターで貰ったポインタ
	Player* player_ = nullptr;
	GameCamera* gameCamera_ = nullptr;

	std::list<std::unique_ptr<EnemySpawner>> enemySpawners_;

};