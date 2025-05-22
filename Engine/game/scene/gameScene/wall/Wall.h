#pragma once

#include "Object3d.h"
#include "Collider.h"

class Wall :public Object3d, Collider
{
public:

	void Init();

	void Update();

	void OnCollisionEnter(Collider* other) override;
	void OnCollisionStay(Collider* other) override;
	void OnCollisionExit(Collider* other) override;

private:


};

