#pragma once
#include <map>
#include <string>
#include <memory>

class DirectXEngine;
class ModelBase;
class Model;

class ModelManager
{
private:
	static ModelManager* instance_;

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;
public:
	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();

	void Initialize(DirectXEngine* dxEngine);

	void Finalize();


	void LoadModel(const std::string& directoryPath, const std::string& filePath);

	Model* FindModel(const std::string& filePath);

private:
	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models_;

	ModelBase* modelBase_ = nullptr;
};