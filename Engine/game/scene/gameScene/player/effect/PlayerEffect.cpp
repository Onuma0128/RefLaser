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

	// PostEffectを初期化
	DirectXEngine::GetPostEffectMgr()->CreatePostEffect(PostEffectType::Grayscale);
	DirectXEngine::GetPostEffectMgr()->CreatePostEffect(PostEffectType::Vignette);

	cylinder_ = std::make_unique<PrimitiveDrawr>();
	cylinder_->TypeInit(PrimitiveType::Cylinder, 32);
	cylinder_->GetTransform().scale = {};
	cylinder_->SetColor({ 1.0f,1.0f,0.0f });
	cylinder_->GetRenderOptions().enabled = false;
	cylinder_->GetRenderOptions().offscreen = false;
}

void PlayerEffect::Update()
{	
	UpdatePostEffect();

	cylinder_->GetTransform().translation = player_->GetTransform().translation_;
	cylinder_->GetTransform().translation.y = 0.0f;
	cylinder_->Update();
}

void PlayerEffect::Draw()
{
	if (cylinder_->GetRenderOptions().enabled) {
		cylinder_->TypeDraw();
	}
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

void PlayerEffect::UpdatePostEffect()
{
	const float expandDuration = 1.0f;  // 60フレーム
	const float holdDuration = 3.0f;	// 180フレーム
	const float shrinkDuration = 1.0f;  // 60フレーム

	float delta = 1.0f / 60.0f;

	switch (specialMoveState_) {
	case SpecialMoveState::Expanding:
		specialMoveFrame_ += delta;
		{
			// 必殺技のフレーム管理
			float t = std::clamp(specialMoveFrame_ / expandDuration, 0.0f, 1.0f);
			t = Easing::EaseInQuint(t);
			// PostEffectへの値を適応
			DirectXEngine::GetPostEffectMgr()->GetGrayscaleData()->t = t;
			DirectXEngine::GetPostEffectMgr()->GetVignetteData()->gamma = t * 0.8f;
			// Cylinderのスケール、回転を適応
			float scale = t * 20.0f;
			cylinder_->GetTransform().scale = { scale ,scale / 2.0f ,scale };
			cylinder_->GetTransform().rotation.y = t * 3.14f;

			if (specialMoveFrame_ >= expandDuration) {
				specialMoveFrame_ = 0.0f;
				specialMoveState_ = SpecialMoveState::Holding;
				cylinder_->GetRenderOptions().enabled = false;
			}
		}
		break;

	case SpecialMoveState::Holding:
		specialMoveFrame_ += delta;
		{
			// PostEffectへの値を適応
			DirectXEngine::GetPostEffectMgr()->GetGrayscaleData()->t = 1.0f;
			DeltaTimer::SetTimeScaleForSeconds(0.1f, 0.5f);

			if (Input::GetInstance()->GetGamepadLeftTrigger() == 0.0f &&
				specialMoveFrame_ >= holdDuration) {
				specialMoveFrame_ = 0.0f;
				specialMoveState_ = SpecialMoveState::Shrinking;
				cylinder_->GetRenderOptions().enabled = true;
			}
		}
		break;

	case SpecialMoveState::Shrinking:
		specialMoveFrame_ += delta;
		{
			// 必殺技のフレーム管理
			float t = std::clamp(1.0f - (specialMoveFrame_ / shrinkDuration), 0.0f, 1.0f);
			t = Easing::EaseInQuint(t);
			// PostEffectへの値を適応
			DirectXEngine::GetPostEffectMgr()->GetGrayscaleData()->t = t;
			DirectXEngine::GetPostEffectMgr()->GetVignetteData()->gamma = t * 0.8f;
			// Cylinderのスケール、回転を適応
			float scale = t * 20.0f;
			cylinder_->GetTransform().scale = { scale ,scale / 2.0f ,scale };
			cylinder_->GetTransform().rotation.y = t * 3.14f;

			if (specialMoveFrame_ >= shrinkDuration) {
				specialMoveFrame_ = 0.0f;
				specialMoveState_ = SpecialMoveState::None;
				isSpecialMove_ = false;
				DirectXEngine::GetPostEffectMgr()->GetGrayscaleData()->t = 0.0f;
				DirectXEngine::GetPostEffectMgr()->GetVignetteData()->gamma = 0.0f;
				cylinder_->GetTransform().scale = {};
				cylinder_->GetRenderOptions().enabled = false;
				player_->SpecialAttackBullet();
			}
		}
		break;

	case SpecialMoveState::None:
		if (isSpecialMove_) {
			specialMoveState_ = SpecialMoveState::Expanding;
			cylinder_->GetRenderOptions().enabled = true;
			DeltaTimer::SetTimeScaleForSeconds(0.1f, 2.0f);
		}
		break;

	default:
		break;
	}
}
