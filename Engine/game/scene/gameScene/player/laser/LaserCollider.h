#pragma once
#include <vector>

#include "Collider.h"

#include "Vector3.h"
#include "Quaternion.h"

class LaserCollider : public Collider
{
public:

	void Init();

	void Update(std::vector<Vector3> positions);

	void SetActive(bool flag) { Collider::isActive_ = flag; }

private:

	void ActiveCollider();

	Quaternion VelocityToQuaternion(const Vector3& velocity, const float lerp);

private:

	float activeFrame_ = 1.0f;

};

