#pragma once
#include <array>

#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "PrimitiveDrawr.h"

#include "WorldTransform.h"

enum class SpecialMoveState {
	None,
	Expanding,   // 拡大中（0→1）
	Holding,     // 維持（1）
	Shrinking    // 縮小中（1→0）
};

class Player;

class PlayerEffect
{
public:

	void Init();

	void Update();

	void Draw();

	void SetPlayer(Player* player) { player_ = player; }

	// 移動時のエフェクトを呼び出す
	void OnceMoveEffect();
	// 避けた時のエフェクトを呼び出す
	void OnceAvoidEffect();
	// 必殺技を撃った時のエフェクト
	void UpdatePostEffect();
	SpecialMoveState GetSpecialState()const { return specialMoveState_; }
	float GetSpecialMoveFrame()const { return specialMoveFrame_; }
	bool GetIsSpecialMove()const { return isSpecialMove_; }
	void SetIsSpecialMove(bool flag) { isSpecialMove_ = flag; }

private:

	Player* player_ = nullptr;

	// パーティクルを管理する
	ParticleManager* particleManager_ = ParticleManager::GetInstance();

	/* ==================== プレイヤーが移動時のエフェクト ==================== */

	// 移動時の土埃
	std::unique_ptr<ParticleEmitter> moveDustEmitter_ = nullptr;

	/* ==================== プレイヤーが避けた時のエフェクト ==================== */

	// 避けた時の土埃
	std::unique_ptr<ParticleEmitter> avoidDustEmitter_ = nullptr;

	/* ==================== プレイヤーが必殺技を撃った時のエフェクト ==================== */

	// シリンダーで必殺の範囲を視覚的に
	std::unique_ptr<PrimitiveDrawr> cylinder_ = nullptr;
	// 必殺技の遷移状態
	SpecialMoveState specialMoveState_ = SpecialMoveState::None;
	// 今必殺技を撃っているか
	bool isSpecialMove_ = false;
	// 必殺技のフレーム
	float specialMoveFrame_ = 0.0f;
};

