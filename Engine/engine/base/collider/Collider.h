#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Vector3.h"
#include "Quaternion.h"

class Line3d;

enum class ColliderType 
{
	Sphere,		// 球体
	Segment,	// 線
	OBB			// 回転直方体
};

class Collider
{
public:

	virtual ~Collider();
	virtual void AddCollider();
	virtual void DrawCollider();
	virtual void Update();

	virtual void OnCollisionEnter(Collider* other) {};
	virtual void OnCollisionStay(Collider* other) {};
	virtual void OnCollisionExit(Collider* other) {};

	virtual ColliderType GetMyColliderType()const { return myType_; }
	virtual ColliderType GetTargetColliderType()const { return targetType_; }
	virtual bool GetActive()const { return isActive_; }

	virtual Vector3 GetSize() const { return size_; }
	virtual Quaternion GetRotate() const { return rotate_; }
	virtual Vector3 GetCenterPosition() const { return centerPosition_; }

	virtual std::string GetColliderName()const { return colliderName_; }
	virtual float GetRadius()const { return radius_; }
	virtual Vector3 GetOrigin()const { return origin_; }
	virtual Vector3 GetDiff()const { return diff_; }

protected:

	// コライダーの名前
	std::string colliderName_ = "name";

	// コライダーのType
	ColliderType myType_ = ColliderType::Sphere;
	ColliderType targetType_ = ColliderType::Sphere;

	// コライダーをON/OFF
	bool isActive_ = true;

	// 中心座標
	Vector3 centerPosition_;
	
	// OBB用(サイズと回転)
	Vector3 size_;
	Quaternion rotate_;

	// Sphere用(半径)
	float radius_;

	// Segment用
	Vector3 origin_;
	Vector3 diff_;

private:

	std::unique_ptr<Line3d> line_ = nullptr;
	std::vector<Vector3> linePositions_;

	std::vector<Vector3> CreateLinePositions();

};
