#include "Object3d.h"

#include "DirectXEngine.h"

#include "CameraManager.h"
#include "Camera.h"
#include "LightManager.h"
#include "ModelManager.h"
#include "Model.h"

#include "CreateBufferResource.h"

void Object3d::Initialize(const std::string& filePath)
{
    object3dBase_ = std::make_unique<Object3dBase>();
    object3dBase_->Initialize();

    transform_ = WorldTransform();

    SetModel(filePath);

    MakeMaterialData();
}

void Object3d::SetSceneRenderer()
{
    renderOptions_ = {
        .enabled = true,
        .offscreen = true
    };
    DirectXEngine::GetSceneRenderer()->SetDrawList(this);
}

void Object3d::SetRemove()
{
    DirectXEngine::GetSceneRenderer()->SetRemove(this);
}

void Object3d::Update()
{
    transform_.TransferMatrix(model_);
}

void Object3d::Draw()
{
    object3dBase_->DrawBase();

    auto commandList = DirectXEngine::GetCommandList();
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, transform_.GetConstBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(3, LightManager::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(4, LightManager::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(5, LightManager::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(6, CameraManager::GetInstance()->GetCameraResource()->GetGPUVirtualAddress());

    if (model_) {
        model_->Draw(false);
    }

}

void Object3d::SetModel(const std::string& filePath)
{
    model_ = ModelManager::GetInstance()->FindModel(filePath);
}

void Object3d::SetTexture(const std::string& directoryPath, const std::string& filePath)
{
    model_->SetTexture(directoryPath, filePath);
}

void Object3d::SetTexture_ENV(const std::string& directoryPath, const std::string& filePath)
{
    model_->SetTexture_ENV(directoryPath, filePath);
}

void Object3d::SetColor(const Vector4& color)
{
    materialData_->color = color;
}

void Object3d::MakeMaterialData()
{
    // マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
    materialResource_ = CreateBufferResource(DirectXEngine::GetDevice(), sizeof(Material)).Get();
    // 書き込むためのアドレスを取得
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    // 今回は白を書き込んでいく
    materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData_->enableLighting = true;
    materialData_->uvTransform = Matrix4x4::Identity();
    materialData_->shininess = 20.0f;
    materialData_->environmentCoefficient = 0;
}