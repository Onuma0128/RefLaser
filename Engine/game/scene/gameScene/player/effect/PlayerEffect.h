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

	void SetPlayer(Player* player) { player_ = player; }

	// 移動時のエフェクトを呼び出す
	void OnceMoveEffect();
	// 避けた時のエフェクトを呼び出す
	void OnceAvoidEffect();

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

};

