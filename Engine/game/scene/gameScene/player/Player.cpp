#include "Player.h"

#include "state/PlayerMoveState.h"

void Player::Init()
{
	Object3d::Initialize("Box.obj");
	Object3d::GetRenderOptions().enabled = false;

	transform_.scale_ = { 0.5f,0.5f,0.5f };
	transform_.translation_ = { 0.0f,0.5f,0.0f };

	ChengeState(std::make_unique<PlayerMoveState>(this));

	effect_ = std::make_unique<PlayerEffect>();
	effect_->SetPlayer(this);
	effect_->Init();

	reticle_ = std::make_unique<PlayerReticle>();
	reticle_->Init();

	BulletInit();
	PredictionObjInit();

	Collider::AddCollider();
	Collider::colliderName_ = "Player";
	Collider::myType_ = ColliderType::OBB;
	Collider::size_ = transform_.scale_;
	Collider::DrawCollider();

	rightStickQuaternion_ = Quaternion::IdentityQuaternion();
}

void Player::GlobalInit()
{

}

void Player::Update()
{
	state_->Update();

	effect_->Update();

	// 今リロードが終わっている弾のを取得する
	size_t bulletCount = 0;
	// 弾の更新,弾UIの更新
	for (auto& bullet : bullets_) {
		bullet->Update();
		// 弾が消えた時のコールバック関数
		bullet->SetOnDeactivateCallback([]() {});
		// 弾のリロードが終わっているならカウントに追加
		if (bullet->GetIsReload()) { ++bulletCount; }
	}
	for (auto& bullet : specialBullets_) {
		bullet->Update();
		// 弾が消えた時のコールバック関数
		bullet->SetOnDeactivateCallback([]() {});
	}

	size_t bulletUICount = 0;
	for (auto& bulletUI : bulletUIs_) {
		bulletUI->Update({});
		++bulletUICount;

		if (bulletCount >= bulletUICount) {
			bulletUI->GetRenderOptions().enabled = true;
		} else {
			bulletUI->GetRenderOptions().enabled = false;
		}
	}

	for (auto& obj : predictionObjects_) {
		obj->Update();
	}

	Collider::rotate_ = transform_.rotation_;
	Collider::centerPosition_ = transform_.translation_;
	Collider::Update();
	Object3d::Update();
}

void Player::Draw()
{
	effect_->Draw();
	
	reticle_->Draw();
}

void Player::ChengeState(std::unique_ptr<PlayerBaseState> newState)
{
	if (state_ != nullptr) {
		state_->Finalize();
	}
	state_ = std::move(newState);
	state_->Init();
}

void Player::OnCollisionEnter(Collider* other)
{
}

void Player::OnCollisionStay(Collider* other)
{
}

void Player::OnCollisionExit(Collider* other)
{
}

void Player::ReloadBullet()
{
	// 動きが終わっている弾からリロードをする
	for (auto& bullet : bullets_) {
		if (!bullet->GetIsActive() && !bullet->GetIsReload()) {
			bullet->Reload();
			break;
		}
	}
}

void Player::AttackBullet()
{
	// リロードが終わっていて動いていない弾を発射する
	for (auto& bullet : bullets_) {
		if (!bullet->GetIsActive() && bullet->GetIsReload()) {
			WorldTransform transform;
			transform.rotation_ = rightStickQuaternion_;
			transform.translation_ = transform_.translation_;
			bullet->Attack(transform);
			break;
		}
	}
}

void Player::SpecialAttackBullet()
{
	if (reticle_->GetEnemyTransforms().empty()) { return; }
	for (auto& bullet : specialBullets_) {
		bullet->Reload();
	}
	// 敵のTransformを取得した分だけ回す
	uint32_t count = 0;
	for (auto& transform : reticle_->GetEnemyTransforms()) {
		transform.translation_ = transform_.translation_;
		specialBullets_[count]->Attack(transform, 50.0f);
		++count;
	}
	reticle_->GetEnemyTransforms().clear();
	reticle_->ResetHitCount();
}

void Player::BulletInit()
{
	// 弾を初期化
	bullets_.resize(6);
	specialBullets_.resize(6);
	bulletUIs_.resize(6);
	for (auto& bullet : bullets_) {
		bullet = std::make_unique<PlayerBullet>();
		bullet->Init();
	}
	for (auto& bullet : specialBullets_) {
		bullet = std::make_unique<PlayerBullet>();
		bullet->Init();
	}
	// 弾UIを初期化
	for (size_t i = 0; i < bulletUIs_.size(); ++i) {
		bulletUIs_[i] = std::make_unique<PlayerBulletUI>();
		bulletUIs_[i]->Init(Vector2{ (i * 32.0f) + 32.0f,32.0f });
	}
}

void Player::PredictionObjInit()
{
	for (size_t i = 0; i < predictionObjects_.size(); ++i) {
		predictionObjects_[i] = std::make_unique<PredictionObject>();
		predictionObjects_[i]->SetPlayer(this);
		predictionObjects_[i]->Init((Vector3::ExprUnitZ * static_cast<float>(i + 1)));
	}
}
