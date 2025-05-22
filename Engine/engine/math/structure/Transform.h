#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"

struct Transform2D {
	Vector2 size;
	float rotate;
	Vector2 position;
};

struct EulerTransform {
	Vector3 scale;
	Vector3 rotation;
	Vector3 translation;
};

struct Transform3D {
	Vector3 scale;
	Quaternion rotation;
	Vector3 translation;
};