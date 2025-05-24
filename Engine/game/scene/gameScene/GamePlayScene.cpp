#include "GamePlayScene.h"

#include "ParticleManager.h"
#include "SceneManager.h"

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

	wallFactory_ = std::make_unique<WallFactory>();
	wallFactory_->Init();

	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->SetPlayer(player_.get());
	enemyManager_->Init();
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update()
{
	demoObj_->Update();

	player_->Update();

	gameCamera_->Update();

	skyBox_->Update();

	wallFactory_->Update();

	enemyManager_->Update();

	ParticleManager::GetInstance()->Update();
}

void GamePlayScene::Draw()
{
}