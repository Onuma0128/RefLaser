#include "MyGame.h"

#include "SceneFactory.h"

#include "SceneManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "DeltaTimer.h"

#include "GlobalVariables.h"

void MyGame::Initialize()
{
	Framework::Initialize();

	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);
	SceneManager::GetInstance()->ChangeScene("Game");

	DeltaTimer::Initialize();

	GlobalVariables::GetInstance()->LoadFiles();
}

void MyGame::Finalize()
{
	winApp_->Finalize();

	Framework::Finalize();
}

void MyGame::Update()
{
	// ImGui受付開始
	imGuiManager_->Begin();

	Framework::Update();
	
	// DeltaTimeの更新
	DeltaTimer::Update();

	// 当たり判定
	directXEngine_->GetCollisionMgr()->CheckAllCollisions();

	// シーンの更新
	SceneManager::GetInstance()->Update();

	// GlobalVariablesの更新
	GlobalVariables::GetInstance()->Update();

	// カメラの更新
	CameraManager::GetInstance()->Debug_ImGui();
	CameraManager::GetInstance()->Update();

	// ライトの更新
	LightManager::GetInstance()->Debug_ImGui();
	LightManager::GetInstance()->Update();

	// ImGui受付終了
	imGuiManager_->End();
}

void MyGame::Draw()
{
	// 描画前の処理
	directXEngine_->PreDraw();

	directXEngine_->GetSceneRenderer()->AllDraw();

	directXEngine_->SwapChainDrawSet();

	directXEngine_->GetSceneRenderer()->OutAllDraw();

	SceneManager::GetInstance()->Draw();

	// ImGuiの描画
	imGuiManager_->Draw();

	// 描画後の処理
	directXEngine_->PostDraw();
}
