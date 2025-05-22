#pragma once
#include <list>
#include <memory>
#include "gameScene/enemy/Enemy.h"

#include "Object3d.h"
#include "Collider.h"

class EnemySpawner : public Object3d,Collider
{
public:

	void Init();

	void Update();

	void Draw();

	void OnCollisionEnter(Collider* other) override;
	void OnCollisionStay(Collider* other) override;
	void OnCollisionExit(Collider* other) override;

	std::list<std::unique_ptr<Enemy>>& GetEnemyList() { return enemys_; }
	
	Player* GetPlayer() { return player_; }
	void SetPlayer(Player* player) { player_ = player; }

	GameCamera* GetGameCamera() { return gameCamera_; }
	void SetGameCamera(GameCamera* camera) { gameCamera_ = camera; }

	void EnemySpawn();

private:

	// ゲッターで貰ったポインタ
	Player* player_ = nullptr;
	GameCamera* gameCamera_ = nullptr;

	// 敵のリスト
	std::list<std::unique_ptr<Enemy>> enemys_;

};

