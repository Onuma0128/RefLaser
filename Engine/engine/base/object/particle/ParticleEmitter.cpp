#include "ParticleEmitter.h"

#include "DeltaTimer.h"

ParticleEmitter::ParticleEmitter(const std::string name)
{
    GlobalInitialize(name);

    emitter_.name = name;
    emitter_.setPosition = { 0.0f,0.0f,0.0f };
    emitter_.transform = { {1.0f,1.0f,1.0f},Quaternion::IdentityQuaternion(),{0.0f,0.0f,0.0f} };
    emitter_.frequency = 0.1f;
    emitter_.frequencyTime = 0.0f;
    accelerationField_.acceleration = { 0.0f,10.0f,0.0f };
    accelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
    accelerationField_.area.max = { 1.0f,1.0f,1.0f };
    // emitter_.frequencyごとに出すパーティクルの個数
    emitter_.count = 3;
    emitter_.emitterSize = {
        .min = {-1.0f,-1.0f,-1.0f},
        .max = {1.0f,1.0f,1.0f}
    };
    emitter_.minScale = { 1.0f,1.0f,1.0f };
    emitter_.maxScale = { 1.0f,1.0f,1.0f };

    emitter_.blendMode_ = 0;
    moveStart_ = true;
    isFieldStart_ = false;
    isCreate_ = true;

    // Emitterの範囲を線で描画
    AABB aabb = emitter_.emitterSize;
    linePosition_ = CreateLineBox(emitter_.emitterSize);

    line_ = std::make_unique<Line3d>();
    line_->Initialize(linePosition_);
}

void ParticleEmitter::GlobalInitialize(const std::string name)
{
    std::string globalName = name + "Emitter";

    // jsonで指定した座標固定を無くすか
    global_->AddValue<bool>(globalName, "1.structEmitter/isLock", false);
    // Emitterの座標
    global_->AddValue<Vector3>(globalName, "1.structEmitter/position", Vector3{});
    // Emitする範囲
    global_->AddValue<Vector3>(globalName, "1.structEmitter/emitterSizeMin", Vector3{ -1.0f,-1.0f,-1.0f });
    global_->AddValue<Vector3>(globalName, "1.structEmitter/emitterSizeMax", Vector3{ 1.0f,1.0f,1.0f });
    // パーティクルが消えるまでの時間
    global_->AddValue<float>(globalName, "1.structEmitter/lifeTime", 3.0f);
    // 発生頻度
    global_->AddValue<float>(globalName, "1.structEmitter/frequency", 0.5f);
    // 発生する時の個数
    global_->AddValue<int>(globalName, "1.structEmitter/count", 3, {}, true);

    // 加速度
    global_->AddValue<Vector3>(globalName, "2.defParticle/acceleration", Vector3{ 0.0f,10.0f,0.0f });
    // サイズ
    global_->AddValue<Vector3>(globalName, "2.defParticle/minScale", Vector3{ 1.0f,1.0f,1.0f });
    global_->AddValue<Vector3>(globalName, "2.defParticle/maxScale", Vector3{ 1.0f,1.0f,1.0f });
    global_->AddValue<Vector3>(globalName, "2.defParticle/endScale", Vector3{ 1.0f,1.0f,1.0f });
    // 速度
    global_->AddValue<Vector3>(globalName, "2.defParticle/minVelocity", Vector3{ 0.0f,0.0f,0.0f });
    global_->AddValue<Vector3>(globalName, "2.defParticle/maxVelocity", Vector3{ 0.0f,0.0f,0.0f });
    // 回転
    global_->AddValue<float>(globalName, "2.defParticle/minRotateZ", 0.0f);
    global_->AddValue<float>(globalName, "2.defParticle/maxRotateZ", 0.0f);
    // 反射する高さ
    global_->AddValue<float>(globalName, "2.defParticle/reflectY", 0.0f);
    // 出てくる色
    global_->AddValue<Vector3>(globalName, "2.defParticle/color", Vector3{ 1.0f,1.0f,1.0f });

    global_->AddValue<Vector3>(globalName, "2.defParticle/uvTranslate", Vector3{ 0.0f,0.0f,0.0f });


    // BlendModeの設定
    std::vector<const char*> blendMode = { "None","Normal","Add", "Subtract","Multiply" };
    global_->AddValue<int>(globalName, "blendMode", 2, blendMode);
    // 動かすか
    global_->AddValue<bool>(globalName, "move", false);
    // 加速度を足すか
    global_->AddValue<bool>(globalName, "field", false);
    // 反射をするか
    global_->AddValue<bool>(globalName, "reflect", false);
    // スケール変化させるか
    global_->AddValue<bool>(globalName, "changeScale", false);

}

void ParticleEmitter::Update()
{
    /*==================== パーティクルの範囲更新 ====================*/

    std::string globalName = emitter_.name + "Emitter";

    // 新たな数値を代入
    Vector3 min = global_->GetValue<Vector3>(globalName, "1.structEmitter/emitterSizeMin");
    Vector3 max = global_->GetValue<Vector3>(globalName, "1.structEmitter/emitterSizeMax");
    // min,maxが最大値を超えていないかclamp
    emitter_.emitterSize.min = {
        std::clamp(min.x,-256.0f,0.0f),std::clamp(min.y,-256.0f,0.0f),std::clamp(min.z,-256.0f,0.0f),
    };
    emitter_.emitterSize.max = {
        std::clamp(max.x,0.0f,256.0f),std::clamp(max.y,0.0f,256.0f),std::clamp(max.z,0.0f,256.0f),
    };

    emitter_.lifeTime = global_->GetValue<float>(globalName, "1.structEmitter/lifeTime");
    emitter_.frequency = global_->GetValue<float>(globalName, "1.structEmitter/frequency");
    emitter_.count = global_->GetValue<int32_t>(globalName, "1.structEmitter/count");

    // Emitterの範囲より少し大きめにFieldを作る
    accelerationField_.area = {
        .min = emitter_.emitterSize.min + emitter_.transform.translation - Vector3{10.0f,10.0f,10.0f},
        .max = emitter_.emitterSize.max + emitter_.transform.translation + Vector3{10.0f,10.0f,10.0f}
    };
    accelerationField_.acceleration = global_->GetValue<Vector3>(globalName, "2.defParticle/acceleration");

    Vector3 emitterPosition = global_->GetValue<Vector3>(globalName, "1.structEmitter/position");
    if (!global_->GetValue<bool>(globalName, "1.structEmitter/isLock")) {
        emitter_.transform.translation = emitterPosition;
    } else {
        Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(emitter_.transform.rotation);
        emitter_.transform.translation = emitter_.setPosition + (emitterPosition).Transform(rotateMatrix);
        accelerationField_.acceleration = Vector3::Transform(accelerationField_.acceleration, rotateMatrix);
    }

    // min,maxが最大値を超えていないかclamp
    min = global_->GetValue<Vector3>(globalName, "2.defParticle/minScale");
    max = global_->GetValue<Vector3>(globalName, "2.defParticle/maxScale");
    Vector3 end = global_->GetValue<Vector3>(globalName, "2.defParticle/endScale");
    emitter_.minScale = {
        std::clamp(min.x,-100.0f,max.x),std::clamp(min.y,-100.0f,max.y),std::clamp(min.z,-100.0f,max.z),
    };
    emitter_.maxScale = {
        std::clamp(max.x,min.x,100.0f),std::clamp(max.y,min.y,100.0f),std::clamp(max.z,min.z,100.0f),
    };
    emitter_.endScale = {
        std::clamp(end.x,0.0f,100.0f),std::clamp(end.y,0.0f,100.0f),std::clamp(end.z,0.0f,100.0f)
    };

    // min,maxが最大値を超えていないかclamp
    min = global_->GetValue<Vector3>(globalName, "2.defParticle/minVelocity");
    max = global_->GetValue<Vector3>(globalName, "2.defParticle/maxVelocity");
    emitter_.minVelocity = {
        std::clamp(min.x,-100.0f,max.x),std::clamp(min.y,-100.0f,max.y),std::clamp(min.z,-100.0f,max.z),
    };
    emitter_.maxVelocity = {
        std::clamp(max.x,min.x,100.0f),std::clamp(max.y,min.y,100.0f),std::clamp(max.z,min.z,100.0f),
    };

    // min,maxが最大値を超えていないかclamp
    float min_z = global_->GetValue<float>(globalName, "2.defParticle/minRotateZ");
    float max_z = global_->GetValue<float>(globalName, "2.defParticle/maxRotateZ");
    emitter_.minRotateZ = std::clamp(min_z, -100.0f, max_z);
    emitter_.maxRotateZ = std::clamp(max_z, min_z, 100.0f);

    // 反射する高さ
    emitter_.reflectY = global_->GetValue<float>(globalName, "2.defParticle/reflectY");

    emitter_.uvTranslation = global_->GetValue<Vector3>(globalName, "2.defParticle/uvTranslate");

    emitter_.blendMode_ = global_->GetValue<int>(globalName, "blendMode");
    // 動いているか
    moveStart_ = global_->GetValue<bool>(globalName, "move");
    isFieldStart_ = global_->GetValue<bool>(globalName, "field");
    isReflect_  = global_->GetValue<bool>(globalName, "reflect");
    isChangeScale_= global_->GetValue<bool>(globalName, "changeScale");

#ifdef _DEBUG

    linePosition_ = CreateLineBox(emitter_.emitterSize);
    for (auto& linePos : linePosition_) {
        Vector3 translate = emitter_.transform.translation;
        Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(emitter_.transform.rotation);
        linePos = linePos.Transform(rotateMatrix) + translate;
    }
    line_->SetPositions(linePosition_);
    line_->Update();

#endif // _DEBUG
}

void ParticleEmitter::Draw()
{
#ifdef _DEBUG

    /*==================== パーティクルの範囲描画 ====================*/
    /*for (auto& line : lines_) {
        line->Draw();
    }*/

    //line_->Draws();
#endif // _DEBUG
}

void ParticleEmitter::CreateParticles(ParticleManager::ParticleGroup& group)
{
    if (moveStart_ && isCreate_) {
        emitter_.frequencyTime += DeltaTimer::GetDeltaTime();
        if (emitter_.frequency <= emitter_.frequencyTime) {
            std::mt19937 randomEngine_(seedGenerator_());
            group.particles.splice(group.particles.end(), Emit(emitter_, randomEngine_));
            emitter_.frequencyTime -= emitter_.frequency;
        }
    }

    if (onceEmit_) {
        onceEmit_ = false;
        std::mt19937 randomEngine_(seedGenerator_());
        group.particles.splice(group.particles.end(), Emit(emitter_, randomEngine_));
    }
}

void ParticleEmitter::UpdateParticle(std::list<ParticleManager::Particle>::iterator& particle)
{
    if (moveStart_) {
        if (IsCollision(accelerationField_.area, particle->transform.translation) && isFieldStart_) {
            particle->velocity += accelerationField_.acceleration * DeltaTimer::GetDeltaTime();
        }
        particle->transform.translation += particle->velocity * DeltaTimer::GetDeltaTime();
        // 反射の処理
        if (isReflect_) {
            if (particle->transform.translation.y <= emitter_.reflectY) {
                particle->velocity.y *= -(3.0f / 4.0f);
            }
        }
        // スケールの処理
        if (isChangeScale_) {
            float t = 1.0f - (particle->currentTime / particle->lifeTime);
            particle->transform.scale = t * particle->offsetScale + (1.0f - t) * emitter_.endScale;
        }
        particle->uvTranslate += emitter_.uvTranslation;

        particle->currentTime += DeltaTimer::GetDeltaTime();

        std::string globalName = emitter_.name + "Emitter";
        particle->color.x = global_->GetValue<Vector3>(globalName, "2.defParticle/color").x;
        particle->color.y = global_->GetValue<Vector3>(globalName, "2.defParticle/color").y;
        particle->color.z = global_->GetValue<Vector3>(globalName, "2.defParticle/color").z;
    }
}

std::list<ParticleManager::Particle> ParticleEmitter::Emit(const Emitter& emitter, std::mt19937& randomEngine)
{
    std::list<ParticleManager::Particle> particles;
    for (uint32_t count = 0; count < emitter.count; ++count) {
        particles.push_back(MakeNewParticle(randomEngine, emitter));
    }
    return particles;
}

ParticleManager::Particle ParticleEmitter::MakeNewParticle(std::mt19937& randomEngine, const Emitter& emitter)
{
    std::uniform_real_distribution<float> distPosX(emitter.emitterSize.min.x, emitter.emitterSize.max.x);
    std::uniform_real_distribution<float> distPosY(emitter.emitterSize.min.y, emitter.emitterSize.max.y);
    std::uniform_real_distribution<float> distPosZ(emitter.emitterSize.min.z, emitter.emitterSize.max.z);

    std::uniform_real_distribution<float> distVelocityX(emitter.minVelocity.x, emitter.maxVelocity.x);
    std::uniform_real_distribution<float> distVelocityY(emitter.minVelocity.y, emitter.maxVelocity.y);
    std::uniform_real_distribution<float> distVelocityZ(emitter.minVelocity.z, emitter.maxVelocity.z);

    std::uniform_real_distribution<float> distScaleX(emitter.minScale.x, emitter.maxScale.x);
    std::uniform_real_distribution<float> distScaleY(emitter.minScale.y, emitter.maxScale.y);
    std::uniform_real_distribution<float> distScaleZ(emitter.minScale.z, emitter.maxScale.z);

    std::uniform_real_distribution<float> distRotate(emitter.minRotateZ, emitter.maxRotateZ);
    ParticleManager::Particle particle{};
    particle.transform.scale = { distScaleX(randomEngine),distScaleY(randomEngine),distScaleZ(randomEngine) };
    particle.offsetScale = particle.transform.scale;
    particle.transform.rotation = { 0.0f,0.0f,distRotate(randomEngine) };
    Vector3 randomTranslate = { distPosX(randomEngine),distPosY(randomEngine) ,distPosZ(randomEngine) };
    Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(emitter.transform.rotation);
    particle.transform.translation = emitter.transform.translation + randomTranslate.Transform(rotateMatrix);
    Vector3 velocity = { distVelocityX(randomEngine),distVelocityY(randomEngine),distVelocityZ(randomEngine) };
    particle.velocity = velocity.Transform(Quaternion::MakeRotateMatrix(emitter.transform.rotation));
    particle.color = { 1.0f,1.0f,1.0f,1.0f };
    particle.lifeTime = emitter.lifeTime;
    particle.currentTime = 0.0f;
    return particle;
}

std::vector<Vector3> ParticleEmitter::CreateLineBox(AABB aabb)
{
    std::vector<Vector3> linePosition = {
        aabb.min,
        { aabb.max.x, aabb.min.y, aabb.min.z },

        aabb.min,
        { aabb.min.x, aabb.max.y, aabb.min.z },

        aabb.min,
        { aabb.min.x, aabb.min.y, aabb.max.z },

        { aabb.min.x, aabb.max.y, aabb.min.z },
        { aabb.max.x, aabb.max.y, aabb.min.z },

        { aabb.min.x, aabb.max.y, aabb.min.z },
        { aabb.min.x, aabb.max.y, aabb.max.z },

        { aabb.min.x, aabb.min.y, aabb.max.z },
        { aabb.max.x, aabb.min.y, aabb.max.z },

        { aabb.max.x, aabb.min.y, aabb.min.z },
        { aabb.max.x, aabb.min.y, aabb.max.z },

        { aabb.min.x, aabb.min.y, aabb.max.z },
        { aabb.min.x, aabb.max.y, aabb.max.z },

        { aabb.max.x, aabb.min.y, aabb.min.z },
        { aabb.max.x, aabb.max.y, aabb.min.z },

        { aabb.min.x, aabb.max.y, aabb.max.z },
        aabb.max,

        { aabb.max.x, aabb.min.y, aabb.max.z },
        aabb.max,

        { aabb.max.x, aabb.max.y, aabb.min.z },
        aabb.max,
    };

    return linePosition;
}


bool ParticleEmitter::IsCollision(const AABB& aabb, const Vector3& point)
{
    if (aabb.min.x < point.x && aabb.max.x > point.x &&
        aabb.min.y < point.y && aabb.max.y > point.y &&
        aabb.min.z < point.z && aabb.max.z > point.z) {
        return true;
    }
    return false;
}