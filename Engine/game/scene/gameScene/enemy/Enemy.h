#pragma once
#include <memory>

#include "Animation.h"
#include "Collider.h"
#include "GlobalVariables.h"

#include "gameScene/enemy/state/EnemyBaseState.h"
#include "gameScene/enemy/effect/EnemyEffect.h"

enum class EnemyType {
	Melee,		// 近距離
	Ranged,		// 遠距離
	Shield		// 盾持ち
};

struct EnemyPrefabData {
	// 共通部分
	float moveSpeed;			// 移動速度(全敵共通)
	float moveLength;			// プレイヤーが近づいて動く距離
	// 近距離
	float M_attackLength;		// 移動から攻撃に変わる距離(近距離)
	float M_attackTimer;		// 攻撃してからの時間(近距離)
	float M_chengeStateTimer;	// 攻撃が終わった後移動ステートに戻る時間(近距離)
	// 遠距離
	float R_attackLength;		// 移動から攻撃に変わる距離(遠距離)
	float R_attackTimer;		// 攻撃してからの時間(遠距離)
	float R_chengeStateTimer;	// 攻撃が終わった後移動ステートに戻る時間(遠距離)
	float bulletSpeed;			// 弾の速さ(遠距離)
	// 盾持ち
	float stopLength;			// 移動から止まるに変わる距離(盾持ち)
	Vector3 shieldScale;		// シールドのサイズ(盾持ち)
	Vector3 shieldPosition;		// シールドの座標(盾持ち)
};

class Player;
class GameCamera;
class EnemyManager;

class Enemy:public Animation,Collider
{
public:

	void Finalize();

	void Init();

	void Update();

	void Draw();

	void ChengeState(std::unique_ptr<EnemyBaseState> newState);

	void OnCollisionEnter(Collider* other) override;
	void OnCollisionStay(Collider* other) override;
	void OnCollisionExit(Collider* other) override;

	// エフェクト
	EnemyEffect* GetEffect() { return effect_.get(); }
	// 敵マネージャ
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
	// プレイヤー
	Player* GetPlayer() { return player_; }
	void SetPlayer(Player* player) { player_ = player; }
	// ゲームカメラ
	GameCamera* GetGameCamera() { return gameCamera_; }
	void SetGameCamera(GameCamera* camera) { gameCamera_ = camera; }

	// 敵のタイプを取得
	EnemyType GetEnemyType()const { return type_; }
	void SetEnemyType(EnemyType type) { type_ = type; }
	// 敵の向いている方向ベクトル
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }

	// jsonで保存時の名前
	std::string GetGroupName()const { return groupName_; }
	// jsonの番号
	int32_t GetGroupNumber()const { return groupNumber_; }
	void SetGroupNumber(int32_t number) {
		groupNumber_ = number;
		// 番号が変わったので名前も変える
		groupName_ = "Enemy" + std::to_string(groupNumber_);
	}

	// エディター上の削除フラグ
	bool GetDelete()const { return isDelete_; }

private:

	// 調整項目の初期化
	void GlobalInit();
	// デバッグ用
	void DebugTransform();
	// プレハブデータを初期化
	void DebugPrefabData();

private:

	// ゲッターで貰ったポインタ
	EnemyManager* enemyManager_ = nullptr;
	Player* player_ = nullptr;
	GameCamera* gameCamera_ = nullptr;

	// 状態遷移
	std::unique_ptr<EnemyBaseState> state_ = nullptr;
	// エフェクト
	std::unique_ptr<EnemyEffect> effect_ = nullptr;
	// 速度
	Vector3 velocity_{};

	// 敵のtypeごとの基本データ
	EnemyPrefabData prefabData_;
	// 敵のタイプ
	EnemyType type_;

	GlobalVariables* global_ = GlobalVariables::GetInstance();
	// jsonで保存時の名前
	std::string groupName_;
	// jsonの番号
	int32_t groupNumber_;
	// 削除するか
	bool isDelete_;

};

