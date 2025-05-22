#include "ParticleManager.h"

#include <numbers>

#include "DirectXEngine.h"
#include "PipelineState.h"
#include "SrvManager.h"
#include "Line3dBase.h"
#include "LightManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "ModelManager.h"

#include "Camera.h"
#include "CreateBufferResource.h"
#include "ParticleEmitter.h"

ParticleManager* ParticleManager::instance_ = nullptr;

ParticleManager* ParticleManager::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new ParticleManager;
    }
    return instance_;
}

void ParticleManager::Initialize(DirectXEngine* dxEngine)
{
    dxEngine_ = dxEngine;
    srvManager_ = SrvManager::GetInstance();

    rootSignature_ = dxEngine_->GetPipelineState()->GetRootSignature(PipelineType::Particle).Get();
    for (int i = 0; i < static_cast<int>(pipelineStates_.size()); ++i) {
        pipelineStates_[i] = dxEngine_->GetPipelineState()->GetPipelineState(
            PipelineType::Particle, PostEffectType::None, static_cast<BlendMode>(i)
        ).Get();
    }

    modelData_ = Model::LoadObjFile("resources", "plane.obj");

    CreateVertexResource();
}

void ParticleManager::Update()
{
    // 各パーティクルグループに対して処理
    for (auto& [name, group] : particleGroups_) {
        // 新しいパーティクルを追加する
        particleGroups_[name].emitter->Update();
        Emit(name);

        uint32_t numInstance = 0;
        // 各パーティクルを更新
        for (auto it = group.particles.begin(); it != group.particles.end();) {
            if (it->lifeTime <= it->currentTime) {
                it = group.particles.erase(it);
                continue;
            }

            // パーティクルのビルボード化
            Matrix4x4 billboardMatrix = Matrix4x4::Rotate(it->transform.rotation) * CameraManager::GetInstance()->GetActiveCamera()->GetWorldMatrix();
            billboardMatrix.m[3][0] = 0.0f;
            billboardMatrix.m[3][1] = 0.0f;
            billboardMatrix.m[3][2] = 0.0f;
            billboardMatrix.m[3][3] = 1.0f;
            Matrix4x4 worldMatrix = Matrix4x4::Scale(it->transform.scale) * billboardMatrix * Matrix4x4::Translate(it->transform.translation);
            Matrix4x4 worldViewMatrix = worldMatrix * CameraManager::GetInstance()->GetActiveCamera()->GetViewMatrix();
            Matrix4x4 worldViewProjectionMatrix = worldViewMatrix * CameraManager::GetInstance()->GetActiveCamera()->GetProjectionMatrix();

            // パーティクルの更新
            group.emitter->UpdateParticle(it);

            // uvTranslate
            group.materialData_->uvTransform = Matrix4x4::Translate(it->uvTranslate);

            float alpha = 1.0f - (it->currentTime / it->lifeTime);
            if (numInstance < kNumMaxInstance) {
                group.instancingData[numInstance].WVP = worldViewProjectionMatrix;
                group.instancingData[numInstance].World = worldViewMatrix;
                group.instancingData[numInstance].color = it->color;
                group.instancingData[numInstance].color.w = alpha;
                ++numInstance;
            }
            ++it;
        }

        group.instanceCount = numInstance;
        std::memcpy(group.instancingData, group.instancingData, sizeof(ParticleForGPU) * numInstance);
    }
}

void ParticleManager::Draw()
{
    auto commandList = dxEngine_->GetCommandList();

    for (const auto& [name, group] : particleGroups_) {

        /*==================== パイプラインの設定 ====================*/
        commandList->SetGraphicsRootSignature(rootSignature_.Get());
        commandList->SetPipelineState(pipelineStates_[particleGroups_[name].emitter->GetBlendMode()].Get());
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        /*==================== パーティクルの描画 ====================*/
        uint32_t textIndex = group.textureIndex;
        commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
        commandList->SetGraphicsRootConstantBufferView(0, group.materialResource_->GetGPUVirtualAddress());
        commandList->SetGraphicsRootConstantBufferView(1, group.instancingResource->GetGPUVirtualAddress());
        SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textIndex);
        SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(3, group.srvIndex);

        // 各パーティクルグループのインスタンスを描画
        commandList->DrawInstanced(UINT(modelData_.vertices.size()), group.instanceCount, 0, 0);
    }

    for (const auto& [name, group] : particleGroups_) {
        group.emitter->Draw();
    }
}

void ParticleManager::Finalize()
{
    delete instance_;
    instance_ = nullptr;
}

void ParticleManager::Clear()
{
    particleGroups_.clear();
}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath, ParticleEmitter* emitter, bool copy)
{
    auto it = particleGroups_.find(name);
    if (it != particleGroups_.end()) {

        if (copy) {
            // 既存グループを複製
            ParticleGroup newGroup = it->second;
            newGroup.emitter = emitter;         
            newGroup.instanceCount = 0;  

            newGroup.instancingResource = CreateBufferResource(
                dxEngine_->GetDevice(),
                sizeof(ParticleForGPU) * kNumMaxInstance).Get();
            newGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&newGroup.instancingData));
            for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
                newGroup.instancingData[i].WVP = Matrix4x4::Identity();
                newGroup.instancingData[i].World = Matrix4x4::Identity();
                newGroup.instancingData[i].color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
            }

            // SRVだけは取り直す
            newGroup.srvIndex = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
            srvManager_->CreateSRVforStructuredBuffer(
                newGroup.srvIndex,
                newGroup.instancingResource.Get(),
                kNumMaxInstance,
                sizeof(ParticleForGPU));

            // 別名を付けて登録
            std::string copyName = name;
            for (int i = 1; particleGroups_.contains(copyName); ++i) {
                copyName = name + std::to_string(i);
            }
            particleGroups_[copyName] = std::move(newGroup);
        }
        return;
    }

    ParticleGroup group;
    group.textureFilePath = textureFilePath;
    group.textureIndex = TextureManager::GetInstance()->GetSrvIndex(group.textureFilePath);

    // パーティクルグループのマテリアル用のリソース,データを作成
    CreateMatrialResource(group);

    // パーティクルグループのインスタンス数とリソースを初期化
    group.instanceCount = 0;
    group.instancingData = nullptr;

    // インスタンスデータ用のバッファリソースを作成
    group.instancingResource = CreateBufferResource(
        dxEngine_->GetDevice(),
        sizeof(ParticleForGPU) * kNumMaxInstance).Get();

    // リソースをマッピングして、インスタンスデータを初期化
    group.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&group.instancingData));
    for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
        group.instancingData[i].WVP = Matrix4x4::Identity();
        group.instancingData[i].World = Matrix4x4::Identity();
        group.instancingData[i].color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
    }
    group.emitter = emitter;

    group.srvIndex = srvManager_->Allocate() + TextureManager::kSRVIndexTop;

    srvManager_->CreateSRVforStructuredBuffer(
        group.srvIndex,
        group.instancingResource.Get(),
        kNumMaxInstance,
        sizeof(ParticleManager::ParticleForGPU)
    );

    // 新しいパーティクルグループを particleGroups_ マップに追加
    particleGroups_[name] = std::move(group);
}

void ParticleManager::Emit(const std::string name)
{
    // 新しいパーティクルを追加する
    particleGroups_[name].emitter->CreateParticles(particleGroups_[name]);
}

void ParticleManager::CreateVertexResource()
{
    // 実際に頂点リソースを作る
    vertexResource_ = CreateBufferResource(dxEngine_->GetDevice(), sizeof(VertexData) * modelData_.vertices.size()).Get();
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void ParticleManager::CreateMatrialResource(ParticleGroup& group)
{
    // マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
    group.materialResource_ = CreateBufferResource(dxEngine_->GetDevice(), sizeof(Material)).Get();
    // 書き込むためのアドレスを取得
    group.materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&group.materialData_));
    // 今回は白を書き込んでいく
    group.materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    group.materialData_->enableLighting = false;
    group.materialData_->uvTransform = Matrix4x4::Identity();
}