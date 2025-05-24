#include "Enemy.h"

#include "DeltaTimer.h"

#include "gameScene/enemy/state/EnemyMoveState.h"
#include "gameScene/player/Player.h"
#include "gameScene/gameCamera/GameCamera.h"
#include "gameScene/enemy/manager/EnemyManager.h"

void Enemy::Finalize()
{
	Animation::SetRemove();
	Collider::RemoveCollider();
}

void Enemy::Init()
{
	GlobalInit();

	Animation::Init("resources/enemy","enemy.gltf");
	Animation::SetSceneRenderer();

	transform_.scale_ = { 0.5f,0.5f,0.5f };
	transform_.translation_ = { 0.0f,0.5f,0.0f };

	ChengeState(std::make_unique<EnemyMoveState>(this));

	effect_ = std::make_unique<EnemyEffect>();
	effect_->SetEnemy(this);
	effect_->Init();

	Collider::AddCollider();
	Collider::myType_ = ColliderType::OBB;
	Collider::colliderName_ = "Enemy";
	Collider::size_ = transform_.scale_;
	Collider::DrawCollider();
}

void Enemy::Update()
{
#ifdef _DEBUG
	type_ = static_cast<EnemyType>(global_->GetValue<int32_t>("EnemyManager", groupName_ + "/Type"));
	isDelete_ = global_->GetValue<bool>("EnemyManager", groupName_ + "/Delete");

	// EnemyManagerのPrefabデータを更新
	DebugPrefabData();
	// フラグで移動管理
	if (!global_->GetValue<bool>("EnemyManager", "isMove")) {
		DebugTransform();
		Collider::centerPosition_ = transform_.translation_;
		Collider::rotate_ = transform_.rotation_;
		Collider::Update();
		Animation::Update();
		return;
	}
#endif // _DEBUG

	state_->Update();

	effect_->Update();

	Collider::centerPosition_ = transform_.translation_;
	Collider::rotate_ = transform_.rotation_;
	Collider::Update();

	Animation::Update();
}

void Enemy::Draw()
{
	effect_->Draw();
}

void Enemy::ChengeState(std::unique_ptr<EnemyBaseState> newState)
{
	if (state_ != nullptr) {
		state_->Finalize();
	}
	state_ = std::move(newState);
	state_->Init();
}

void Enemy::OnCollisionEnter(Collider* other)
{
}

void Enemy::OnCollisionStay(Collider* other)
{
	// プレイヤーと当たっているなら
	if (other->GetColliderName() == "Player") {
		//Object3d::SetColor(Vector4{ 1.0f,0.0f,0.0f,1.0f });
		const float speed = 2.0f;
		transform_.translation_ -= velocity_ * speed * DeltaTimer::GetDeltaTime();
		Animation::Update();
	}

	// 敵と当たっているなら
	if (other->GetColliderName() == "Enemy") {
		//Object3d::SetColor(Vector4{ 0.0f,1.0f,0.0f,1.0f });
		const float speed = 1.0f;
		Vector3 velocity = transform_.translation_ - other->GetCenterPosition();
		velocity.y = 0.0f;
		if (velocity.Length() != 0.0f) { velocity = velocity.Normalize(); }
		transform_.translation_ += velocity * speed * DeltaTimer::GetDeltaTime();
		Animation::Update();
	}
}

void Enemy::OnCollisionExit(Collider* other)
{
	// プレイヤーと当たっているなら
	// 敵と当たっているなら
	if (other->GetColliderName() == "Player" ||
		other->GetColliderName() == "Enemy") {
		Animation::SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });
		Animation::Update();
	}
}
void Enemy::GlobalInit()
{
	// タイプ
	std::vector<const char*> typeNames = { "Melee", "Ranged","Shield" };
	global_->AddValue<int32_t>("EnemyManager", groupName_ + "/Type", 0, typeNames);
	// 座標
	global_->AddValue<Vector3>("EnemyManager", groupName_ + "/position", { 0.0f,0.0f,0.0f });
	// 削除するか
	global_->AddValue<bool>("EnemyManager", groupName_ + "/Delete", false);
}

void Enemy::DebugTransform()
{
	transform_.scale_ = { 0.5f,0.5f,0.5f };
	transform_.rotation_ = Quaternion::IdentityQuaternion();
	transform_.translation_ = global_->GetValue<Vector3>("EnemyManager", groupName_ + "/position");
}

void Enemy::DebugPrefabData()
{
	// 全体
	prefabData_.moveSpeed = enemyManager_->GetPrefabData().moveSpeed;
	prefabData_.moveLength = enemyManager_->GetPrefabData().moveLength;
	// 近距離
	prefabData_.M_attackLength = enemyManager_->GetPrefabData().M_attackLength;
	prefabData_.M_attackTimer = enemyManager_->GetPrefabData().M_attackTimer;
	prefabData_.M_chengeStateTimer = enemyManager_->GetPrefabData().M_chengeStateTimer;
	// 遠距離
	prefabData_.bulletSpeed = enemyManager_->GetPrefabData().bulletSpeed;
	prefabData_.R_attackLength = enemyManager_->GetPrefabData().R_attackLength;
	prefabData_.R_attackTimer = enemyManager_->GetPrefabData().R_attackTimer;
	prefabData_.R_chengeStateTimer = enemyManager_->GetPrefabData().R_chengeStateTimer;
	// 盾持ち
	prefabData_.stopLength = enemyManager_->GetPrefabData().stopLength;
	prefabData_.shieldScale = enemyManager_->GetPrefabData().shieldScale;
	prefabData_.shieldPosition = enemyManager_->GetPrefabData().shieldPosition;
}
