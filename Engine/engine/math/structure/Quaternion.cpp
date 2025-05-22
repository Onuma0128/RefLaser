#include "Quaternion.h"

#include <cmath>
#include <algorithm>

#include "imgui.h"
#include "Vector3.h"
#include "Matrix4x4.h"

Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

void Quaternion::ImGuiQuaternion(const std::string& imguiName) const
{
	ImGui::Text("%.3f, %.3f, %.3f, %.3f : ", x, y, z, w);
	ImGui::SameLine();
	ImGui::Text(imguiName.c_str());
}

Quaternion Quaternion::IdentityQuaternion()
{
	return { 0.0f,0.0f,0.0f,1.0f };
}

void Quaternion::AddRotation(const Quaternion& deltaRotation)
{
	*this = Normalize(*this * deltaRotation);
}

Quaternion Quaternion::Conjugate(const Quaternion& quaternion)
{
	return { -quaternion.x,-quaternion.y,-quaternion.z,quaternion.w };
}

float Quaternion::Norm(const Quaternion& quaternion)
{
	return std::sqrt(
		quaternion.x * quaternion.x + quaternion.y * quaternion.y + 
		quaternion.z * quaternion.z + quaternion.w * quaternion.w
	);
}

float Quaternion::Dot(const Quaternion& q0, const Quaternion& q1)
{
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

Quaternion Quaternion::Normalize(const Quaternion& quaternion)
{
	float norm = Norm(quaternion);

	Quaternion result = quaternion;

	if (norm > 0.0f)
	{
		result.x /= norm;
		result.y /= norm;
		result.z /= norm;
		result.w /= norm;
	}

	return result;
}

Quaternion Quaternion::Inverse(const Quaternion& quaternion)
{
	float norm = Norm(quaternion) * Norm(quaternion);

	Quaternion conjugate = Conjugate(quaternion);

	return {
		conjugate.x / norm,
		conjugate.y / norm,
		conjugate.z / norm,
		conjugate.w / norm
	};
}

Quaternion Quaternion::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle)
{
	Vector3 axisNormal = axis.Normalize();

	float halfAngle = angle / 2.0f;

	float cosAngle = std::cos(halfAngle);
	float sinAngle = std::sin(halfAngle);

	return {
		axisNormal.x * sinAngle,
		axisNormal.y * sinAngle,
		axisNormal.z * sinAngle,
		cosAngle
	};
}

Vector3 Quaternion::RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	Quaternion conjugate = Conjugate(quaternion);

	Quaternion vectorQuat(vector.x, vector.y, vector.z, 0.0f);

	Quaternion rotatedQuat = quaternion * vectorQuat * conjugate;

	return { rotatedQuat.x, rotatedQuat.y, rotatedQuat.z };
}

Matrix4x4 Quaternion::MakeRotateMatrix(const Quaternion& quaternion)
{
	Quaternion q = quaternion;
	Matrix4x4 result = Matrix4x4::Identity();

	result.m[0][0] = q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z;
	result.m[0][1] = 2.0f * (q.x * q.y + q.w * q.z);
	result.m[0][2] = 2.0f * (q.x * q.z - q.w * q.y);
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * (q.x * q.y - q.w * q.z);
	result.m[1][1] = q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z;
	result.m[1][2] = 2.0f * (q.y * q.z + q.w * q.x);
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * (q.x * q.z + q.w * q.y);
	result.m[2][1] = 2.0f * (q.y * q.z - q.w * q.x);
	result.m[2][2] = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Quaternion Quaternion::FormRotationMatrix(const Matrix4x4& matrix)
{
	Quaternion result = IdentityQuaternion();

	float trace = matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2];

	if (trace > 0.0f) {
		float s = std::sqrt(trace + 1.0f) * 2.0f; // s = 4 * w
		result.w = 0.25f * s;
		result.x = (matrix.m[2][1] - matrix.m[1][2]) / s;
		result.y = (matrix.m[0][2] - matrix.m[2][0]) / s;
		result.z = (matrix.m[1][0] - matrix.m[0][1]) / s;
	}
	else {
		// 最大要素を基準にしてゼロ除算を防ぐ
		if ((matrix.m[0][0] > matrix.m[1][1]) && (matrix.m[0][0] > matrix.m[2][2])) {
			float s = std::sqrt(1.0f + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2]) * 2.0f;
			result.w = (matrix.m[2][1] - matrix.m[1][2]) / s;
			result.x = 0.25f * s;
			result.y = (matrix.m[0][1] + matrix.m[1][0]) / s;
			result.z = (matrix.m[0][2] + matrix.m[2][0]) / s;
		}
		else if (matrix.m[1][1] > matrix.m[2][2]) {
			float s = std::sqrt(1.0f + matrix.m[1][1] - matrix.m[0][0] - matrix.m[2][2]) * 2.0f;
			result.w = (matrix.m[0][2] - matrix.m[2][0]) / s;
			result.x = (matrix.m[0][1] + matrix.m[1][0]) / s;
			result.y = 0.25f * s;
			result.z = (matrix.m[1][2] + matrix.m[2][1]) / s;
		}
		else {
			float s = std::sqrt(1.0f + matrix.m[2][2] - matrix.m[0][0] - matrix.m[1][1]) * 2.0f;
			result.w = (matrix.m[1][0] - matrix.m[0][1]) / s;
			result.x = (matrix.m[0][2] + matrix.m[2][0]) / s;
			result.y = (matrix.m[1][2] + matrix.m[2][1]) / s;
			result.z = 0.25f * s;
		}
	}

	return Normalize(result);
}

Quaternion Quaternion::ExtractYawQuaternion(const Quaternion& quaternion)
{
	Vector3 forward = Quaternion::RotateVector(Vector3::ExprUnitZ, quaternion);
	float yaw = atan2(forward.x, forward.z); // Y 軸の回転成分を取得
	return Quaternion::MakeRotateAxisAngleQuaternion(Vector3::ExprUnitY, yaw);
}

Vector3 Quaternion::ToEuler(const Quaternion& q) {
	Vector3 euler;

	// Pitch（X軸回転）
	float sinp = 2.0f * (q.w * q.x + q.y * q.z);
	float cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	euler.x = std::atan2(sinp, cosp); // pitch

	// Yaw（Y軸回転）
	float siny = 2.0f * (q.w * q.y - q.z * q.x);
	siny = std::clamp(siny, -1.0f, 1.0f); // asin範囲制限
	euler.y = std::asin(siny); // yaw

	// Roll（Z軸回転）
	float sinr = 2.0f * (q.w * q.z + q.x * q.y);
	float cosr = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	euler.z = std::atan2(sinr, cosr); // roll

	return euler;
}

void Quaternion::Slerp(const Quaternion& q1, float t)
{
	float dot = Dot(*this, q1);
	if (dot < 0) {
		*this = -*this;
		dot = -dot;
	}

	if (dot >= 1.0f - FLT_EPSILON) {
		*this = *this * (1.0f - t) + q1 * t;
		return;
	}

	float theta = std::acos(dot);

	float scale0 = std::sin((1.0f - t) * theta) / std::sin(theta);
	float scale1 = std::sin(t * theta) / std::sin(theta);

	*this = *this * scale0 + q1 * scale1;
}

Quaternion Quaternion::Lerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	t = std::clamp(t, 0.0f, 1.0f); // t を 0.0 ~ 1.0 に制限
	Quaternion result(
		q0.x + (q1.x - q0.x) * t,
		q0.y + (q1.y - q0.y) * t,
		q0.z + (q1.z - q0.z) * t,
		q0.w + (q1.w - q0.w) * t
	);
	return Normalize(result);
}

Quaternion Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	Quaternion copyQ0 = q0;
	float dot = Dot(q0, q1);
	if (dot < 0) {
		copyQ0 = -copyQ0;
		dot = -dot;
	}

	if (dot >= 1.0f - FLT_EPSILON) {
		copyQ0 = copyQ0 * (1.0f - t) + q1 * t;
		return Normalize(copyQ0);
	}

	float theta = std::acos(dot);

	float scale0 = std::sin((1.0f - t) * theta) / std::sin(theta);
	float scale1 = std::sin(t * theta) / std::sin(theta);

	return copyQ0 * scale0 + q1 * scale1;
}

Quaternion Quaternion::operator-() const
{
	return { -x, -y, -z , -w };
}

Quaternion Quaternion::operator+(const Quaternion& q) const
{
	return { x + q.x,q.y + y,q.z + z,q.w + w };
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
	return {
		y * q.z - z * q.y + q.w * x + w * q.x,
		z * q.x - x * q.z + q.w * y + w * q.y,
		x * q.y - y * q.x + q.w * z + w * q.z,
		w * q.w - x * q.x - y * q.y - z * q.z
	};
}

Quaternion Quaternion::operator*(float scalar) const
{
	return { x * scalar,y * scalar,z * scalar,w * scalar };
}

Quaternion& Quaternion::operator+=(const Quaternion& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
	return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& q)
{
	*this = *this * q;
	return *this;
}

Quaternion& Quaternion::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}
