#include "WallFactory.h"

#include "Line3d.h"

void WallFactory::Init()
{
	// 調整項目の初期化
	GlobalInit();
	const int32_t spownCount = global_->GetValue<int32_t>("WallFactory", "create,delete");

	// 敵の初期化
	for (int32_t i = 0; i < spownCount; ++i) {
		// 敵の初期化
		std::unique_ptr<Wall> wall = std::make_unique<Wall>();
		std::string charaName = "WallObject" + std::to_string(i);
		wall->SetGroupNumber(i);
		wall->Init();
		wallObjects_.push_back(std::move(wall));
	}

	// カウントの初期化
	int32_t nowObjectCount = global_->GetValue<int32_t>("WallFactory", "create,delete");
	objectAddCount_ = nowObjectCount;
	objectSubtractCount_ = nowObjectCount;

	// 敵配置位置のObject
	Object3d::Initialize("Box.obj");
	Object3d::SetSceneRenderer();
	Object3d::GetRenderOptions().enabled = false;
#ifdef _DEBUG
	Object3d::GetRenderOptions().enabled = true;
#endif // _DEBUG

}

void WallFactory::Update()
{
#ifdef _DEBUG
	// 配置の位置を可視化
	transform_.translation_ = global_->GetValue<Vector3>("WallFactory", "position");
	Object3d::Update();
	// 敵を追加する
	CreateObject();
	// 敵を削除する
	DeleteObject();
#endif // _DEBUG

	for (auto& wall : wallObjects_) {
		wall->Update();
	}
}

void WallFactory::Finalize()
{
}

void WallFactory::GlobalInit()
{
	global_->AddValue<Vector3>("WallFactory", "position", { 0,0,0 });
	global_->AddValue<int32_t>("WallFactory", "create,delete", 0, {}, true);
}

void WallFactory::CreateObject()
{
	int32_t nowObjectCount = global_->GetValue<int32_t>("WallFactory", "create,delete");
	if (objectAddCount_ < nowObjectCount) {
		// 敵の初期化
		std::unique_ptr<Wall> wall = std::make_unique<Wall>();
		std::string charaName = "WallObject" + std::to_string(static_cast<int32_t>(wallObjects_.size() + 1));
		wall->SetGroupNumber(nowObjectCount);
		wall->Init();
		Vector3 position = global_->GetValue<Vector3>("WallFactory", "position");
		wall->GetTransform().translation_ = position;
		global_->SetValue<Vector3>("WallFactory", charaName + "/position", position);
		wallObjects_.push_back(std::move(wall));
	}

	objectAddCount_ = nowObjectCount;
}

void WallFactory::DeleteObject()
{
	// stageObjectsの最後のgroup名を取得
	std::string lastGroupName;
	if (!wallObjects_.empty()) {
		lastGroupName = wallObjects_.back()->GetGroupName();
	}

	// ボタンを押した時にobjectを削除
	int32_t nowObjectCount = global_->GetValue<int32_t>("WallFactory", "create,delete");
	if (objectSubtractCount_ > nowObjectCount) {
		if (!wallObjects_.empty()) {
			// 最後のobjectのグループ名を取得して削除
			global_->RemoveSubGroup("WallFactory", lastGroupName);
			// リストから削除
			wallObjects_.back()->Finalize();
			wallObjects_.pop_back();

			objectSubtractCount_ = nowObjectCount;
		}
	}

	// エディター上で削除される前にjsonのgroupの繰り上げをする
	std::string deleteName;
	int32_t deleteNumber = 0;
	for (auto& wall : wallObjects_) {
		// deleteがされているならgroupNumberを取得
		if (wall->GetDelete()) {
			deleteName = wall->GetGroupName();
			deleteNumber = wall->GetGroupNumber();
		} else {
			// groupの繰り上げ
			if (deleteNumber != 0) {
				std::string name = wall->GetGroupName();
				wall->SetGroupNumber(deleteNumber);
				// 繰り上げ
				Vector3 scale = global_->GetValue<Vector3>("WallFactory", name + "/scale");
				float rotateY = global_->GetValue<float>("WallFactory", name + "/rotateY");
				Vector3 position = global_->GetValue<Vector3>("WallFactory", name + "/position");
				global_->SetValue<Vector3>("WallFactory", deleteName + "/scale", scale);
				global_->SetValue<float>("WallFactory", deleteName + "/rotateY", rotateY);
				global_->SetValue<Vector3>("WallFactory", deleteName + "/position", position);
				global_->SetValue<bool>("WallFactory", deleteName + "/Delete", false);
				// 加算
				deleteNumber++;
				deleteName = name;
			}
		}
	}

	// エディター上でdeleteされているなら削除
	wallObjects_.erase(
		std::remove_if(wallObjects_.begin(), wallObjects_.end(),
			[&nowObjectCount, &lastGroupName](const std::unique_ptr<Wall>& object) {
				if (object->GetDelete()) {
					// jsonのgroupを削除する
					GlobalVariables::GetInstance()->RemoveSubGroup("WallFactory", lastGroupName);
					object->Finalize();

					// objectカウントを減算
					nowObjectCount--;
					GlobalVariables::GetInstance()->SetValue<int32_t>("WallFactory", "create,delete", nowObjectCount);
					return true;
				}
				return false;
			}),
		wallObjects_.end());

	objectSubtractCount_ = nowObjectCount;
}
