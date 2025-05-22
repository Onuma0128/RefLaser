#include "EnemySpawner.h"

#include "gameScene/player/Player.h"
#include "gameScene/gameCamera/GameCamera.h"

void EnemySpawner::Init()
{
	Object3d::Initialize("Box.obj");
	Object3d::SetSceneRenderer();
	//Object3d::GetRenderOptions().enabled = false;

	Collider::AddCollider();
	Collider::myType_ = ColliderType::OBB;
	Collider::colliderName_ = "EnemySpawner";
	Collider::size_ = transform_.scale_;
	Collider::DrawCollider();
}

void EnemySpawner::Update()
{
	for (auto& enemy : enemys_) {
		enemy->Update();
	}

	Collider::rotate_ = transform_.rotation_;
	Collider::centerPosition_ = transform_.translation_;
	Collider::Update();

	Object3d::Update();
}

void EnemySpawner::Draw()
{
	for (auto& enemy : enemys_) {
		enemy->Draw();
	}
}

void EnemySpawner::OnCollisionEnter(Collider* other)
{
}

void EnemySpawner::OnCollisionStay(Collider* other)
{
	if (other->GetColliderName() == "LaserLine") {
		Object3d::SetColor(Vector4{ 1.0f,0.0f,0.0f,1.0f });
	}
}

void EnemySpawner::OnCollisionExit(Collider* other)
{
	if (other->GetColliderName() == "LaserLine") {
		Object3d::SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });
	}
}

void EnemySpawner::EnemySpawn()
{
	// 敵を生成
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->SetPlayer(player_);
	enemy->SetGameCamera(gameCamera_);
	enemy->Init();
	enemy->GetTransform().translation_ = transform_.translation_;
	enemys_.push_back(std::move(enemy));
}
