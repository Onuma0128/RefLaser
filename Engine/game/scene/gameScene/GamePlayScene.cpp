#include "GamePlayScene.h"

#include "ParticleManager.h"
#include "SceneManager.h"

#include "gameScene/enemy/Enemy.h"

#include "Collision3D.h"
#include "Vector3.h"

void GamePlayScene::Initialize()
{
	demoObj_ = std::make_unique<Object3d>();
	demoObj_->Initialize("plane.obj");
	demoObj_->SetSceneRenderer();
	demoObj_->SetColor({ 0.1f,0.1f,0.1f,1.0f });
	demoObj_->GetTransform().scale_ = { 20.0f,20.0f,20.0f };
	demoObj_->GetTransform().rotation_ = Quaternion::MakeRotateAxisAngleQuaternion(Vector3::ExprUnitX, -1.57f);

	skyBox_ = std::make_unique<PrimitiveDrawr>();
	skyBox_->TypeInit(PrimitiveType::Skybox);
	skyBox_->GetTransform().scale = { 512.0f,512.0f ,512.0f };
	//skyBox_->GetRenderOptions().enabled = false;

	player_ = std::make_unique<Player>();
	player_->Init();

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->SetPlayer(player_.get());
	gameCamera_->Init();

	enemySpawnerFactory_ = std::make_unique<EnemySpawnerFactory>();
	enemySpawnerFactory_->SetPlayer(player_.get());
	enemySpawnerFactory_->SetGameCamera(gameCamera_.get());
	enemySpawnerFactory_->Init();
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update()
{
	demoObj_->Update();

	player_->Update();

	enemySpawnerFactory_->Update();

	gameCamera_->Update();

	skyBox_->Update();

	ParticleManager::GetInstance()->Update();
}

void GamePlayScene::Draw()
{
	enemySpawnerFactory_->Draw();

	player_->Draw();
}