#include "PlayerBulletEffect.h"

void PlayerBulletEffect::Init()
{
	// 弾のエフェクト
	bulletTrailEmitter_ = std::make_unique<ParticleEmitter>("bulletTrail");
	particleManager_->CreateParticleGroup("bulletTrail", "white1x1.png", bulletTrailEmitter_.get(), true);
	bulletTrailEmitter_->SetIsCreate(false);

	bulletDeleteEmitter_ = std::make_unique<ParticleEmitter>("bulletDelete");
	particleManager_->CreateParticleGroup("bulletDelete", "white1x1.png", bulletDeleteEmitter_.get(), true);
	bulletDeleteEmitter_->SetIsCreate(false);

	// ヒット時のエフェクト
	hitEmitter_ = std::make_unique<ParticleEmitter>("enemyHit");
	particleManager_->CreateParticleGroup("enemyHit", "white1x1.png", hitEmitter_.get(), true);
	hitEmitter_->SetIsCreate(false);

	hitDustEmitter_ = std::make_unique<ParticleEmitter>("enemyHitExplosion");
	particleManager_->CreateParticleGroup("enemyHitExplosion", "circle.png", hitDustEmitter_.get(), true);
	hitDustEmitter_->SetIsCreate(false);

	// 弾を撃つ時のエフェクト
	bulletExplosionEmitter_ = std::make_unique<ParticleEmitter>("bulletExplosion");
	particleManager_->CreateParticleGroup("bulletExplosion", "circle.png", bulletExplosionEmitter_.get(), true);
	bulletExplosionEmitter_->SetIsCreate(false);

	bulletSparkEmitter_ = std::make_unique<ParticleEmitter>("bulletSpark");
	particleManager_->CreateParticleGroup("bulletSpark", "circle.png", bulletSparkEmitter_.get(), true);
	bulletSparkEmitter_->SetIsCreate(false);

	bulletSmokeEmitter_ = std::make_unique<ParticleEmitter>("bulletSmoke");
	particleManager_->CreateParticleGroup("bulletSmoke", "smoke.png", bulletSmokeEmitter_.get(), true);
	bulletSmokeEmitter_->SetIsCreate(false);

	bulletCartridgeEmitter_ = std::make_unique<ParticleEmitter>("bulletCartridge");
	particleManager_->CreateParticleGroup("bulletCartridge", "white1x1.png", bulletCartridgeEmitter_.get(), true);
	bulletCartridgeEmitter_->SetIsCreate(false);
}

void PlayerBulletEffect::Update()
{
}

void PlayerBulletEffect::OnceBulletTrailEffect(const WorldTransform& transform)
{
	bulletTrailEmitter_->onceEmit();

	// パーティクルの座標を設定
	Quaternion rotate = transform.rotation_;
	Vector3 position = transform.translation_;

	bulletTrailEmitter_->SetRotation(rotate);
	bulletTrailEmitter_->SetPosition(position);
}

void PlayerBulletEffect::OnceBulletDeleteEffect(const WorldTransform& transform)
{
	bulletDeleteEmitter_->onceEmit();

	// パーティクルの座標を設定
	Quaternion rotate = transform.rotation_;
	Vector3 position = transform.translation_;

	bulletDeleteEmitter_->SetRotation(rotate);
	bulletDeleteEmitter_->SetPosition(position);
}

void PlayerBulletEffect::OnceBulletHitEffect(const WorldTransform& transform)
{
	hitEmitter_->onceEmit();
	hitDustEmitter_->onceEmit();

	// パーティクルの座標を設定
	Quaternion rotate = transform.rotation_;
	Vector3 position = transform.translation_;

	hitEmitter_->SetRotation(rotate);
	hitEmitter_->SetPosition(position);
	hitDustEmitter_->SetRotation(rotate);
	hitDustEmitter_->SetPosition(position);
}

void PlayerBulletEffect::OnceBulletEffect(const WorldTransform& transform)
{
	// Particleを一回生成
	bulletExplosionEmitter_->onceEmit();
	bulletSparkEmitter_->onceEmit();
	bulletSmokeEmitter_->onceEmit();
	bulletCartridgeEmitter_->onceEmit();

	// パーティクルの座標を設定
	Quaternion rotate = transform.rotation_;
	Vector3 position = transform.translation_;

	// 爆発
	bulletExplosionEmitter_->SetPosition(position);
	bulletExplosionEmitter_->SetRotation(rotate);
	// 火花
	bulletSparkEmitter_->SetPosition(position);
	bulletSparkEmitter_->SetRotation(rotate);
	// 煙
	bulletSmokeEmitter_->SetPosition(position);
	bulletSmokeEmitter_->SetRotation(rotate);
	// 薬莢
	bulletCartridgeEmitter_->SetPosition(position);
	bulletCartridgeEmitter_->SetRotation(rotate);
}