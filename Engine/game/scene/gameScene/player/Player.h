#pragma once
#include <memory>
#include <array>
#include <vector>

#include "Object3d.h"
#include "Collider.h"

#include "gameScene/player/state/PlayerBaseState.h"
#include "gameScene/player/effect/PlayerEffect.h"
#include "gameScene/player/bullet/PredictionObject.h"
#include "gameScene/player/bullet/PlayerBullet.h"
#include "gameScene/player/ui/PlayerBulletUI.h"
#include "gameScene/player/reticle/PlayerReticle.h"

class Player : public Object3d,Collider
{
public:

	void Init();

	void GlobalInit();

	void Update();

	void Draw();

	void ChengeState(std::unique_ptr<PlayerBaseState> newState);

	void OnCollisionEnter(Collider* other) override;
	void OnCollisionStay(Collider* other) override;
	void OnCollisionExit(Collider* other) override;

	// 弾のリロードを管理する関数
	void ReloadBullet();

	// 弾を発射する関数
	void AttackBullet();
	void SpecialAttackBullet();

	PlayerEffect* GetEffect() { return effect_.get(); }
	PlayerReticle* GetReticle() { return reticle_.get(); }

	std::vector<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

	Quaternion GetRightStickQua()const { return rightStickQuaternion_; }
	void SetRightStickQua(const Quaternion& q) { rightStickQuaternion_ = q; }

private:

	void BulletInit();
	void PredictionObjInit();

private:

	// 状態遷移
	std::unique_ptr<PlayerBaseState> state_ = nullptr;
	// エフェクト
	std::unique_ptr<PlayerEffect> effect_ = nullptr;

	// 弾を撃つ方向の回転
	Quaternion rightStickQuaternion_{};
	// 予測の点のオブジェクト
	std::array<std::unique_ptr<PredictionObject>, 3> predictionObjects_;
	// 必殺技時にロックオンするためのレティクル
	std::unique_ptr<PlayerReticle> reticle_ = nullptr;
	// 弾を6つ生成する
	std::vector<std::unique_ptr<PlayerBullet>> bullets_;
	std::vector<std::unique_ptr<PlayerBulletUI>> bulletUIs_;
	// 必殺技用の弾を6つ生成
	std::vector<std::unique_ptr<PlayerBullet>> specialBullets_;

};