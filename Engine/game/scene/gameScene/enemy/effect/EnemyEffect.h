#pragma once
#include <memory>

#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "PrimitiveDrawr.h"

#include "WorldTransform.h"

class Enemy;

class EnemyEffect
{
public:

	void Init();

	void Update();

	void Draw();

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

private:

	void HitReticleUpdate();

private:

	Enemy* enemy_ = nullptr;

	// パーティクルを管理する
	ParticleManager* particleManager_ = ParticleManager::GetInstance();


};