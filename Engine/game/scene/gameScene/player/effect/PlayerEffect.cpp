#include "PlayerEffect.h"

#include "DirectXEngine.h"
#include "PostEffectManager.h"
#include "DeltaTimer.h"
#include "Input.h"

#include "Easing.h"
#include "gameScene/player/Player.h"

void PlayerEffect::Init()
{
	// 移動時のエフェクト
	moveDustEmitter_ = std::make_unique<ParticleEmitter>("moveDust");
	particleManager_->CreateParticleGroup("moveDust", "smoke.png", moveDustEmitter_.get());
	moveDustEmitter_->SetIsCreate(false);

	// 避け時のエフェクト
	avoidDustEmitter_ = std::make_unique<ParticleEmitter>("avoidDust");
	particleManager_->CreateParticleGroup("avoidDust", "smoke.png", avoidDustEmitter_.get());
	avoidDustEmitter_->SetIsCreate(false);

}

void PlayerEffect::Update()
{	
}

void PlayerEffect::OnceMoveEffect()
{
	moveDustEmitter_->onceEmit();

	// パーティクルの座標を設定
	Quaternion rotate = player_->GetTransform().rotation_;
	Vector3 position = player_->GetTransform().translation_;
	
	moveDustEmitter_->SetRotation(rotate);
	moveDustEmitter_->SetPosition(position);
}

void PlayerEffect::OnceAvoidEffect()
{
	avoidDustEmitter_->onceEmit();

	// パーティクルの座標を設定
	Quaternion rotate = player_->GetTransform().rotation_;
	Vector3 position = player_->GetTransform().translation_;

	avoidDustEmitter_->SetPosition(position);
	avoidDustEmitter_->SetRotation(rotate);
}