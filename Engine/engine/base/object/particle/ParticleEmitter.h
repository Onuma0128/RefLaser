#pragma once

#include <iostream>
#include <random>
#include <memory>
#include <list>
#include <vector>
#include <unordered_map>

#include "Line3d.h"

#include "Transform.h"
#include "Vector3.h"

#include "ParticleManager.h"
#include "GlobalVariables.h"

class ParticleEmitter
{
public:

	struct AABB {
		Vector3 min;
		Vector3 max;
	};

	struct Emitter {

		/* ==================== Emitter ==================== */

		std::string name;			// 何のParticleを保持しているかのname
		Vector3 setPosition;		// Emitterを固定する座標
		Transform3D transform;		// EmitterのTransform
		AABB emitterSize;			// Emitterのsize

		/* ==================== Paritcle ==================== */

		Vector3 minScale;			// Paritcle発生時の最小size
		Vector3 maxScale;			// Paritcle発生時の最大size
		Vector3 endScale;			// Paritcleが消える時のsize

		Vector3 minVelocity;		// Paritcle発生時の最小velocity
		Vector3 maxVelocity;		// Paritcle発生時の最大velocity

		float minRotateZ;			// Particle発生時の最小rotateZ
		float maxRotateZ;			// Particle発生時の最大rotateZ
		float reflectY;				// Particleが反射するY軸を設定

		Vector3 uvTranslation;		// ParticleのUVをスクロールする

		uint32_t blendMode_;		// ParticleのBlendModeを設定
		uint32_t count;				// Paritcle発生時に何個出すか
		float lifeTime;				// Particle発生時から生きている時間
		float frequency;			// Particle発生するまでの時間
		float frequencyTime;		// Particle発生するまでカウントするTime
	};

	struct AccelerationField {
		Vector3 acceleration;
		AABB area;
	};

public:

	ParticleEmitter(const std::string name);

	void GlobalInitialize(const std::string name);

	void Update();

	void Draw();

	/*==================== パーティクルの生成 ====================*/

	void CreateParticles(ParticleManager::ParticleGroup& group);

	void UpdateParticle(std::list<ParticleManager::Particle>::iterator& particle);

	// 呼び出す事で一回だけEmitする
	void onceEmit() { onceEmit_ = true; }

	/*==================== アクセッサー ====================*/

	void SetRotation(const Quaternion& rotation) { emitter_.transform.rotation = rotation; }

	void SetPosition(const Vector3& position) { emitter_.setPosition = position; }

	void SetAcceleration(const Vector3& acceleration) { accelerationField_.acceleration = acceleration; }

	void SetIsCreate(bool isCreate) { isCreate_ = isCreate; }

	int GetBlendMode()const { return emitter_.blendMode_; }

private:

	static std::list<ParticleManager::Particle> Emit(const Emitter& emitter, std::mt19937& randomEngine);

	static ParticleManager::Particle MakeNewParticle(std::mt19937& randomEngine, const Emitter& emitter);

	std::vector<Vector3> CreateLineBox(AABB aabb);

	bool IsCollision(const AABB& aabb, const Vector3& point);

private:

	GlobalVariables* global_ = GlobalVariables::GetInstance();

	/*==================== エミッター ====================*/

	// 乱数生成器の初期化
	std::random_device seedGenerator_;

	// エミッタの範囲を可視化
	std::vector<Vector3> linePosition_;
	std::vector<std::unique_ptr<Line3d>> lines_;
	std::unique_ptr<Line3d> line_;

	// Transform変数を作る
	AccelerationField accelerationField_{};
	Emitter emitter_{};

	bool moveStart_ = false;
	bool isFieldStart_ = false;
	bool isCreate_ = true;
	bool onceEmit_ = false;
	bool isReflect_ = false;
	bool isChangeScale_ = false;
};