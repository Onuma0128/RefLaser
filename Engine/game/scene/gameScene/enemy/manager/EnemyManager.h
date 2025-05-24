#pragma once
#include <list>
#include <memory>

#include "Object3d.h"

#include "GlobalVariables.h"

#include "gameScene/enemy/Enemy.h"

class Player;

class EnemyManager : public Object3d
{
public:

	void Init();

	void Update();

	void Finalize();

	void SetPlayer(Player* player) { player_ = player; }

	EnemyPrefabData GetPrefabData()const { return prefabData_; }

private:

	// 調整項目の初期化
	void GlobalInit();
	// 調整項目の初期化(敵のテンプレート)
	void EnemyPrefabGlobalInit(const std::string& name);
	void SetEnemyPrefabGlobal(const std::string& name, const std::string& setName);
	void SetPrefabData();

	// 敵を追加する
	void CreateEnemy();
	// 敵を削除する
	void DeleteEnemy();

	// 敵Prefabを追加する
	void CreateEnemyPrefab();
	// 敵Prefabを削除する
	void DeleteEnemyPrefab();

private:

	// json保存
	GlobalVariables* global_ = GlobalVariables::GetInstance();

	// EnemyPrefabの名前を保存
	std::vector<std::string> prefabNamesStr_;
	std::vector<const char*> prefabNames_;

	// 敵を管理するlist
	std::list<std::unique_ptr<Enemy>> enemys_;

	// Playerのポインタ
	Player* player_ = nullptr;

	// Prefabのデータ
	EnemyPrefabData prefabData_;


	/* ==============================  ============================== */


	// 現在の敵の数を取得
	int32_t enemyAddCount_ = 0;
	int32_t enemySubtractCount_ = 0;

	// 現在の敵prefabの数を取得
	int32_t enemyPrefabAddCount_ = 0;
	int32_t enemyPrefabSubtractCount_ = 0;

};