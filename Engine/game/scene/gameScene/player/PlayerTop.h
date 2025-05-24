#pragma once

#include "Object3d.h"

#include "Quaternion.h"

class PlayerTop : public Object3d
{
public:

	void Init();

	void Update(const Quaternion& q);

private:


};
