#include "PlayerSpecialMoveState.h"

#include "DirectXEngine.h"
#include "PostEffectManager.h"
#include "Input.h"
#include "DeltaTimer.h"
#include "Easing.h"

#include "gameScene/player/Player.h"
#include "gameScene/player/state/PlayerMoveState.h"
#include "gameScene/player/effect/PlayerEffect.h"

PlayerSpecialMoveState::PlayerSpecialMoveState(Player* player) :PlayerBaseState(player) {}

void PlayerSpecialMoveState::Init()
{
	player_->GetReticle()->GetTransform().size = { 0.0f,0.0f };
	player_->GetReticle()->GetTransform().position = { 640.0f,300.0f };
	player_->GetReticle()->GetRenderOptions().enabled = true;
	player_->GetReticle()->SetColliderActive(true);
}

void PlayerSpecialMoveState::Finalize()
{
}

void PlayerSpecialMoveState::Update()
{
	switch (player_->GetEffect()->GetSpecialState())
	{
	case SpecialMoveState::Expanding:
	{
		// 必殺技のGrayScaleが広がる時
		float t = std::clamp((player_->GetEffect()->GetSpecialMoveFrame() - 0.5f) * 2.0f, 0.0f, 1.0f);
		t = Easing::EaseOutBack(t);
		player_->GetReticle()->GetTransform().size = Vector2{ 64.0f,64.0f }*t;
	}
		break;
	case SpecialMoveState::Shrinking:
	{
		// 必殺技のGrayScaleが縮まる時
		float t = std::clamp(1.0f - (player_->GetEffect()->GetSpecialMoveFrame() / 0.5f), 0.0f, 1.0f);
		t = Easing::EaseOutBack(t);
		player_->GetReticle()->GetTransform().size = Vector2{ 64.0f,64.0f }*t;
		player_->GetReticle()->SetColliderActive(false);

		// 必殺技が終わったらターゲットしている敵に弾を出す
		if (t <= 0.0f) {
			player_->SpecialAttackBullet();
			player_->GetReticle()->GetRenderOptions().enabled = false;
			player_->ChengeState(std::make_unique<PlayerMoveState>(player_));
			return;
		}
	}
		break;
	default:
		break;
	}

	// レティクルの更新
	player_->GetReticle()->Update();
}

void PlayerSpecialMoveState::Draw()
{
}
