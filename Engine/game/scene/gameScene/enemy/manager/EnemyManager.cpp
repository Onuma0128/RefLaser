#include "EnemyManager.h"

#include "gameScene/player/Player.h"

void EnemyManager::Init()
{
	// 調整項目の初期化
	GlobalInit();

	// カウントの初期化
	const int32_t spownCount = global_->GetValue<int32_t>("EnemyManager", "Enemy : create,delete");
	enemyAddCount_ = spownCount;
	enemySubtractCount_ = spownCount;

	int32_t prefabCount = global_->GetValue<int32_t>("EnemyManager", "EnemyPrefab : create,delete");
	for (int32_t i = 0; i < prefabCount; ++i) {
		std::string name = "EnemyPrefab" + std::to_string(i + 1);
		EnemyPrefabGlobalInit(name);
	}
	enemyPrefabAddCount_ = prefabCount;
	enemyPrefabSubtractCount_ = prefabCount;

	// prefabDataをセットする
	SetPrefabData();

	// 敵の初期化
	for (int32_t i = 0; i < spownCount; ++i) {
		// 敵の初期化
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
		std::string charaName = "Enemy" + std::to_string(i + 1);
		enemy->SetGroupNumber(i + 1);
		enemy->SetEnemyManager(this);
		enemy->SetPlayer(player_);
		enemy->Init();
		enemys_.push_back(std::move(enemy));
	}

	// 敵配置位置のObject
	Object3d::Initialize("Box.obj");
	Object3d::SetSceneRenderer();
	transform_.scale_ = { 0.5f,0.5f,0.5f };
}

void EnemyManager::Update()
{
#ifdef _DEBUG
	// 配置の位置を可視化
	transform_.translation_ = global_->GetValue<Vector3>("EnemyManager", "position");
	Object3d::Update();
	// 敵を追加、削除する
	CreateEnemy();
	DeleteEnemy();
	// 敵prefabを追加、削除する
	CreateEnemyPrefab();
	DeleteEnemyPrefab();
	// prefabDataをセットする
	SetPrefabData();

#endif // _DEBUG

	for (auto& enemy : enemys_) {
		enemy->Update();
	}
}

void EnemyManager::Finalize()
{

}

void EnemyManager::GlobalInit()
{
	// EnemyManager内で持つGlobalVariables
	global_->AddValue<int32_t>("EnemyManager", "EnemyPrefab : create,delete", 0, {}, true);
	int32_t count = global_->GetValue<int32_t>("EnemyManager", "EnemyPrefab : create,delete");
	prefabNamesStr_.push_back({ "" });
	for (int32_t i = 0; i < count; ++i) {
		prefabNamesStr_.emplace_back("EnemyPrefab" + std::to_string(i + 1));
	}
	for (const auto& str : prefabNamesStr_) {
		prefabNames_.push_back(str.c_str());
	}
	global_->AddValue<int32_t>("EnemyManager", "SetEnemyPrefab", 0, prefabNames_);

	// Enemyを作る時設定する値
	std::vector<const char*> typeNames = { "Melee", "Ranged","Shield" };
	global_->AddValue<int32_t>("EnemyManager", "Type", 0, typeNames);
	global_->AddValue<Vector3>("EnemyManager", "position", { 0,0,0 });
	global_->AddValue<int32_t>("EnemyManager", "Enemy : create,delete", 0, {}, true);
	global_->AddValue<bool>("EnemyManager", "isMove", true);
}

void EnemyManager::EnemyPrefabGlobalInit(const std::string& name)
{
	// Prefab内で持つGlobalVariables
	// 全体
	global_->AddValue<float>(name, "Template/moveSpeed", 1.0f);
	global_->AddValue<float>(name, "Template/moveLength", 10.0f);
	// 近距離
	global_->AddValue<float>(name, "Melee/attackLength", 1.0f);
	global_->AddValue<float>(name, "Melee/attackTimer", 1.0f);
	global_->AddValue<float>(name, "Melee/chengeStateTimer", 1.0f);
	// 遠距離
	global_->AddValue<float>(name, "Ranged/bulletSpeed", 1.0f);
	global_->AddValue<float>(name, "Ranged/attackLength", 5.0f);
	global_->AddValue<float>(name, "Ranged/attackTimer", 1.0f);
	global_->AddValue<float>(name, "Ranged/chengeStateTimer", 1.0f);
	// 盾持ち
	global_->AddValue<float>(name, "Shield/stopLength", 3.0f);
	global_->AddValue<Vector3>(name, "Shield/shieldScale", Vector3{ 1.0f,1.0f,1.0f });
	global_->AddValue<Vector3>(name, "Shield/shieldPosition", Vector3{ 0.0f,0.0f,1.0f });
	// 削除フラグ
	global_->AddValue<bool>(name, "Delete", false);
}

void EnemyManager::SetEnemyPrefabGlobal(const std::string& name, const std::string& setName)
{
	// 全体
	float speed = global_->GetValue<float>(setName, "Template/moveSpeed");
	float moveLength = global_->GetValue<float>(setName, "Template/moveSpeed");
	// 近距離
	float M_attackLength = global_->GetValue<float>(setName, "Melee/attackLength");
	float M_attackTimer = global_->GetValue<float>(setName, "Melee/attackTimer");
	float M_chengeStateTimer = global_->GetValue<float>(setName, "Melee/chengeStateTimer");
	// 遠距離
	float bulletSpeed = global_->GetValue<float>(setName, "Ranged/bulletSpeed");
	float R_attackLength = global_->GetValue<float>(setName, "Ranged/attackLength");
	float R_attackTimer = global_->GetValue<float>(setName, "Ranged/attackTimer");
	float R_chengeStateTimer = global_->GetValue<float>(setName, "Ranged/chengeStateTimer");
	// 盾持ち
	float stopLength = global_->GetValue<float>(setName, "Shield/stopLength");
	Vector3 shieldScale = global_->GetValue<Vector3>(setName, "Shield/shieldScale");
	Vector3 shieldPosition = global_->GetValue<Vector3>(setName, "Shield/shieldPosition");

	// アクセッサで値を変更
	// 全体
	global_->SetValue<float>(name, "Template/moveSpeed", speed);
	global_->SetValue<float>(name, "Template/moveLength", moveLength);
	// 近距離
	global_->SetValue<float>(name, "Melee/attackLength", M_attackLength);
	global_->SetValue<float>(name, "Melee/attackTimer", M_attackTimer);
	global_->SetValue<float>(name, "Melee/chengeStateTimer", M_chengeStateTimer);
	// 遠距離
	global_->SetValue<float>(name, "Ranged/bulletSpeed", bulletSpeed);
	global_->SetValue<float>(name, "Ranged/attackLength", R_attackLength);
	global_->SetValue<float>(name, "Ranged/attackTimer", R_attackTimer);
	global_->SetValue<float>(name, "Ranged/chengeStateTimer", R_chengeStateTimer);
	// 盾持ち
	global_->SetValue<float>(name, "Shield/stopLength", stopLength);
	global_->SetValue<Vector3>(name, "Shield/shieldScale", shieldScale);
	global_->SetValue<Vector3>(name, "Shield/shieldPosition", shieldPosition);
	// 削除フラグ(デフォルトでfalse)
	global_->SetValue<bool>(name, "Delete", false);
}

void EnemyManager::SetPrefabData()
{
	int32_t setPrefabCount = global_->GetValue<int32_t>("EnemyManager", "SetEnemyPrefab");
	int32_t prefabButton = global_->GetValue<int32_t>("EnemyManager", "EnemyPrefab : create,delete");
	std::string prefabName = "EnemyPrefab" + std::to_string(setPrefabCount);

	// Prefab登録されているなら
	if (setPrefabCount != 0 && prefabButton != 0) {
		// 全体
		prefabData_.moveSpeed = global_->GetValue<float>(prefabName, "Template/moveSpeed");
		prefabData_.moveLength = global_->GetValue<float>(prefabName, "Template/moveLength");
		// 近距離
		prefabData_.M_attackLength = global_->GetValue<float>(prefabName, "Melee/attackLength");
		prefabData_.M_attackTimer = global_->GetValue<float>(prefabName, "Melee/attackTimer");
		prefabData_.M_chengeStateTimer = global_->GetValue<float>(prefabName, "Melee/chengeStateTimer");
		// 遠距離
		prefabData_.bulletSpeed = global_->GetValue<float>(prefabName, "Ranged/bulletSpeed");
		prefabData_.R_attackLength = global_->GetValue<float>(prefabName, "Ranged/attackLength");
		prefabData_.R_attackTimer = global_->GetValue<float>(prefabName, "Ranged/attackTimer");
		prefabData_.R_chengeStateTimer = global_->GetValue<float>(prefabName, "Ranged/chengeStateTimer");
		// 盾持ち
		prefabData_.stopLength = global_->GetValue<float>(prefabName, "Shield/stopLength");
		prefabData_.shieldScale = global_->GetValue<Vector3>(prefabName, "Shield/shieldScale");
		prefabData_.shieldPosition = global_->GetValue<Vector3>(prefabName, "Shield/shieldPosition");

		// Prefab登録されていなければ
	} else {
		// 全体
		prefabData_.moveSpeed = 1.0f;
		prefabData_.moveLength = 10.0f;
		// 近距離
		prefabData_.M_attackLength = 1.0f;
		prefabData_.M_attackTimer = 1.0f;
		prefabData_.M_chengeStateTimer = 1.0f;
		// 遠距離
		prefabData_.bulletSpeed = 1.0f;
		prefabData_.R_attackLength = 5.0f;
		prefabData_.R_attackTimer = 1.0f;
		prefabData_.R_chengeStateTimer = 1.0f;
		// 盾持ち
		prefabData_.stopLength = 3.0f;
		prefabData_.shieldScale = Vector3{ 1.0f,1.0f,1.0f };
		prefabData_.shieldPosition = Vector3{ 0.0f,0.0f,1.0f };
	}
}

void EnemyManager::CreateEnemy()
{
	int32_t nowEnemyCount = global_->GetValue<int32_t>("EnemyManager", "Enemy : create,delete");
	if (enemyAddCount_ < nowEnemyCount) {
		// 敵の初期化
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
		std::string charaName = "Enemy" + std::to_string(static_cast<int32_t>(enemys_.size() + 1));
		enemy->SetGroupNumber(nowEnemyCount);
		enemy->SetEnemyManager(this);
		enemy->Init();
		enemy->SetPlayer(player_);
		// EnemyManagerで設定した値でEnemyを作成
		EnemyType type = static_cast<EnemyType>(global_->GetValue<int32_t>("EnemyManager", "Type"));
		Vector3 position = global_->GetValue<Vector3>("EnemyManager", "position");
		enemy->SetEnemyType(type);
		enemy->GetTransform().translation_ = position;
		global_->SetValue<int32_t>("EnemyManager", charaName + "/Type", static_cast<int32_t>(type));
		global_->SetValue<Vector3>("EnemyManager", charaName + "/position", position);
		enemys_.push_back(std::move(enemy));
	}

	enemyAddCount_ = nowEnemyCount;
}

void EnemyManager::DeleteEnemy()
{
	// enemysの最後のgroup名を取得
	std::string lastGroupName;
	if (!enemys_.empty()) {
		lastGroupName = enemys_.back()->GetGroupName();
	}

	// ボタンを押した時にenemyを削除
	int32_t nowEnemyCount = global_->GetValue<int32_t>("EnemyManager", "Enemy : create,delete");
	if (enemySubtractCount_ > nowEnemyCount) {
		if (!enemys_.empty()) {
			// 最後のenemyのグループ名を取得して削除
			global_->RemoveSubGroup("EnemyManager", lastGroupName);
			// リストから削除
			enemys_.back()->Finalize();
			enemys_.pop_back();

			enemySubtractCount_ = nowEnemyCount;
		}
	}

	// エディター上で削除される前にjsonのgroupの繰り上げをする
	std::string deleteName;
	int32_t deleteNumber = 0;
	for (auto& enemy : enemys_) {
		// deleteがされているならgroupNumberを取得
		if (enemy->GetDelete()) {
			deleteName = enemy->GetGroupName();
			deleteNumber = enemy->GetGroupNumber();
		} else {
			// groupの繰り上げ
			if (deleteNumber != 0) {
				std::string name = enemy->GetGroupName();
				enemy->SetGroupNumber(deleteNumber);
				// 繰り上げ
				int32_t type = global_->GetValue<int32_t>("EnemyManager", name + "/Type");
				Vector3 position = global_->GetValue<Vector3>("EnemyManager", name + "/position");
				global_->SetValue<int32_t>("EnemyManager", deleteName + "/Type", type);
				global_->SetValue<Vector3>("EnemyManager", deleteName + "/position", position);
				global_->SetValue<bool>("EnemyManager", deleteName + "/Delete", false);
				// 加算
				deleteNumber++;
				deleteName = name;
			}
		}
	}

	// エディター上でdeleteされているなら削除
	enemys_.erase(
		std::remove_if(enemys_.begin(), enemys_.end(),
			[&nowEnemyCount, &lastGroupName](const std::unique_ptr<Enemy>& enemy) {
				if (enemy->GetDelete()) {
					enemy->Finalize();
					// jsonのgroupを削除する
					GlobalVariables::GetInstance()->RemoveSubGroup("EnemyManager", lastGroupName);

					// enemyカウントを減算
					nowEnemyCount--;
					GlobalVariables::GetInstance()->SetValue<int32_t>("EnemyManager", "Enemy : create,delete", nowEnemyCount);
					return true;
				}
				return false;
			}),
		enemys_.end());

	enemySubtractCount_ = nowEnemyCount;
}

void EnemyManager::CreateEnemyPrefab()
{
	int32_t nowEnemyCount = global_->GetValue<int32_t>("EnemyManager", "EnemyPrefab : create,delete");
	if (enemyPrefabAddCount_ < nowEnemyCount) {
		// prefabを追加
		// prefabの名前を作成
		prefabNamesStr_.emplace_back("EnemyPrefab" + std::to_string(nowEnemyCount));
		prefabNames_.clear();
		for (const auto& str : prefabNamesStr_) {
			prefabNames_.push_back(str.c_str());
		}

		// 調整項目の初期化(敵デフォルト)
		EnemyPrefabGlobalInit(prefabNamesStr_.back());

		// prefab選択に追加
		int32_t num = global_->GetValue<int32_t>("EnemyManager", "SetEnemyPrefab");
		global_->SetValue<int32_t>("EnemyManager", "SetEnemyPrefab", num, prefabNames_);
	}

	enemyPrefabAddCount_ = nowEnemyCount;
}

void EnemyManager::DeleteEnemyPrefab()
{
	// prefabsの最後のgroup名を取得
	std::string lastGroupName;
	if (!prefabNamesStr_.empty()) {
		lastGroupName = prefabNamesStr_.back();
	}

	// ボタンを押した時にenemyを削除
	int32_t nowPrefabCount = global_->GetValue<int32_t>("EnemyManager", "EnemyPrefab : create,delete");
	if (enemyPrefabSubtractCount_ > nowPrefabCount) {
		if (!prefabNamesStr_.empty()) {
			// 最後のenemyのグループ名を取得して削除
			global_->DeleteGroup(lastGroupName);
			// リストから削除
			prefabNamesStr_.pop_back();
			// リストを削除して入れ直す
			prefabNames_.clear();
			for (const auto& str : prefabNamesStr_) {
				prefabNames_.push_back(str.c_str());
			}
			// prefab選択を詰める
			int32_t num = global_->GetValue<int32_t>("EnemyManager", "SetEnemyPrefab");
			global_->SetValue<int32_t>("EnemyManager", "SetEnemyPrefab", num, prefabNames_);

			enemyPrefabSubtractCount_ = nowPrefabCount;
		}
	}

	bool isDelete = false;
	int32_t prefabCount = 1;
	for (auto& prefabName : prefabNamesStr_) {
		if (prefabCount == 1) {
			prefabCount++;
			continue;
		}
		// prefabのdeleteフラグを取得
		if (!isDelete) {
			isDelete = global_->GetValue<bool>(prefabName, "Delete");
		}
		// フラグが立っているなら
		if (isDelete && prefabCount <= (prefabNamesStr_.size() - 1)) {
			std::string nextName = "EnemyPrefab" + std::to_string(prefabCount);
			SetEnemyPrefabGlobal(prefabName, nextName);
		}
		prefabCount++;
	}

	// deleteフラグが立ったら
	if (isDelete) {
		// 最後のenemyのグループ名を取得して削除
		global_->DeleteGroup(lastGroupName);
		// リストから削除
		prefabNamesStr_.pop_back();
		// リストを削除して入れ直す
		prefabNames_.clear();
		for (const auto& str : prefabNamesStr_) {
			prefabNames_.push_back(str.c_str());
		}
		// prefab選択を詰める
		int32_t num = global_->GetValue<int32_t>("EnemyManager", "SetEnemyPrefab");
		if ((prefabNamesStr_.size() - 1) > num) {
			global_->SetValue<int32_t>("EnemyManager", "SetEnemyPrefab", num, prefabNames_);
		} else {
			num = static_cast<int32_t>(prefabNamesStr_.size() - 1);
			global_->SetValue<int32_t>("EnemyManager", "SetEnemyPrefab", num, prefabNames_);
		}

		nowPrefabCount--;
		global_->SetValue<int32_t>("EnemyManager", "EnemyPrefab : create,delete", nowPrefabCount);
	}


	enemyPrefabSubtractCount_ = nowPrefabCount;
}
