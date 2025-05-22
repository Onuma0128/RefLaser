#pragma once
#include <memory>
#include <vector>

#include "WorldTransform.h"
#include "Collider.h"
#include "Sprite.h"

class PlayerReticle :public Sprite,Collider
{
public:

	void Init();

	void GlobalInit();

	void Update();

	void Draw();

	void OnCollisionEnter(Collider* other) override;
	void OnCollisionStay(Collider* other) override;
	void OnCollisionExit(Collider* other) override;

	void SetColliderActive(bool flag) { Collider::isActive_ = flag; }

	std::vector<WorldTransform>& GetEnemyTransforms() { return enemyTransforms_; }
	void ResetHitCount() { hitCount_ = 0; }

private:

	void SegmentUpdate();

private:

	// 敵のTransformを取得用
	std::vector<WorldTransform> enemyTransforms_;
	// 何体敵と当たったかカウント用
	uint32_t hitCount_ = 0;

};

