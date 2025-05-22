#pragma once

#include <array>

#include "ParticleManager.h"
#include "ParticleEmitter.h"

#include "WorldTransform.h"

class PlayerBulletEffect
{
public:

	void Init();

	void Update();

	// 弾のトレイルエフェクトを呼び出す
	void OnceBulletTrailEffect(const WorldTransform& transform);
	// 弾が消える時のエフェクトを呼び出す
	void OnceBulletDeleteEffect(const WorldTransform& transform);
	// 弾がヒットした時のエフェクトを呼び出す
	void OnceBulletHitEffect(const WorldTransform& transform);
	// 弾のエフェクトを呼び出す
	void OnceBulletEffect(const WorldTransform& transform);

private:

	// パーティクルを管理する
	ParticleManager* particleManager_ = ParticleManager::GetInstance();

	/* ==================== プレイヤーが弾を撃った時のエフェクト ==================== */

	// 弾のトレイルエフェクト
	std::unique_ptr<ParticleEmitter> bulletTrailEmitter_;
	// 弾が消える時のエフェクト
	std::unique_ptr<ParticleEmitter> bulletDeleteEmitter_;

	/* ==================== 敵に弾がヒットした時のエフェクト ==================== */

	// 弾がヒットした時のエフェクト(血飛沫)
	std::unique_ptr<ParticleEmitter> hitEmitter_;
	// 弾がヒットした時のエフェクト(土煙)
	std::unique_ptr<ParticleEmitter> hitDustEmitter_;

	/* ==================== プレイヤーが弾を撃った時のエフェクト ==================== */

	// 弾を撃った時の爆発
	std::unique_ptr<ParticleEmitter> bulletExplosionEmitter_ = nullptr;
	// 弾を撃った時の火花
	std::unique_ptr<ParticleEmitter> bulletSparkEmitter_ = nullptr;
	// 弾を撃った後の煙
	std::unique_ptr<ParticleEmitter> bulletSmokeEmitter_ = nullptr;
	// 弾を撃った後の薬莢
	std::unique_ptr<ParticleEmitter> bulletCartridgeEmitter_ = nullptr;
};

