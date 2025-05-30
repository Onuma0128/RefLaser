#pragma once
#include <memory>

#include "BaseScene.h"

#include "Object3d.h"
#include "Animation.h"
#include "PrimitiveDrawr.h"

#include "gameScene/player/Player.h"
#include "gameScene/gameCamera/GameCamera.h"
#include "gameScene/wall/WallFactory.h"
#include "gameScene/enemy/manager/EnemyManager.h"


class GamePlayScene : public BaseScene
{
public:

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:

	std::unique_ptr<Object3d> demoObj_ = nullptr;
	std::unique_ptr<PrimitiveDrawr> skyBox_ = nullptr;
	
	// プレイヤーの生成
	std::unique_ptr<Player> player_ = nullptr;

	// ゲームカメラの生成
	std::unique_ptr<GameCamera> gameCamera_ = nullptr;

	// 壁の生成エディタ
	std::unique_ptr<WallFactory> wallFactory_ = nullptr;

	// 敵の生成エディタ
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;

};

