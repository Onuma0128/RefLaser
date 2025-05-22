#include "ModelManager.h"

#include "DirectXEngine.h"
#include "ModelBase.h"
#include "Model.h"

ModelManager* ModelManager::instance_ = nullptr;

ModelManager* ModelManager::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new ModelManager;
    }
    return instance_;
}

void ModelManager::Initialize(DirectXEngine* dxEngine)
{
    modelBase_ = ModelBase::GetInstance();
    modelBase_->Initialize(dxEngine);
}

void ModelManager::Finalize()
{
    ModelBase::GetInstance()->Finalize();
    delete instance_;
    instance_ = nullptr;
}

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& filePath)
{
    if (models_.contains(filePath)) {
        // 読み込み済みなら早期return
        return;
    }
    // モデルの生成とファイル読み込み、初期化
    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->Initialize(directoryPath, filePath);

    // モデルをmapコンテナに格納する
    models_.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
    if (models_.contains(filePath)) {
        // 読み込みモデルを戻り値としてreturn
        return models_.at(filePath).get();
    }
    return nullptr;
}
