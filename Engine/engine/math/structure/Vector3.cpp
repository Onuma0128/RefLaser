#include "Vector3.h"

#include <algorithm>

// コンストラクタ
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

const Vector3 Vector3::ExprUnitX = { 1.0f,0.0f,0.0f };
const Vector3 Vector3::ExprUnitY = { 0.0f,1.0f,0.0f };
const Vector3 Vector3::ExprUnitZ = { 0.0f,0.0f,1.0f };
const Vector3 Vector3::ExprZero = { 0.0f,0.0f,0.0f };

// 加算
Vector3 Vector3::Add(const Vector3& v1, const Vector3& v2) {
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

// 減算
Vector3 Vector3::Subtract(const Vector3& v1, const Vector3& v2) {
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

// スカラー倍
Vector3 Vector3::Multiply(float scalar, const Vector3& v) {
    return { v.x * scalar, v.y * scalar, v.z * scalar };
}

float Vector3::Length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

// 内積
float Vector3::Dot(const Vector3& v1, const Vector3& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// 外積
Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2) {
    return {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return Vector3(
        v1.x + (v2.x - v1.x) * t,
        v1.y + (v2.y - v1.y) * t,
        v1.z + (v2.z - v1.z) * t
    );
}

// 正規化
Vector3 Vector3::Normalize() const {
    float length = Length();
    assert(length != 0 && "正規化中にゼロ除算が発生しました。");
    return { x / length, y / length, z / length };
}

Vector3 Vector3::Transform(const Vector3& v, const Matrix4x4& matrix)
{
    Vector3 result{};
    result.x = v.x * matrix.m[0][0] + v.y * matrix.m[1][0] + v.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
    result.y = v.x * matrix.m[0][1] + v.y * matrix.m[1][1] + v.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
    result.z = v.x * matrix.m[0][2] + v.y * matrix.m[1][2] + v.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
    float w = v.x * matrix.m[0][3] + v.y * matrix.m[1][3] + v.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
    assert(w != 0.0f);
    result.x /= w;
    result.y /= w;
    result.z /= w;
    return result;
}

// 行列による変換（仮実装）
Vector3 Vector3::Transform(const Matrix4x4& matrix) const {
    Vector3 result{};
    result.x = x * matrix.m[0][0] + y * matrix.m[1][0] + z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
    result.y = x * matrix.m[0][1] + y * matrix.m[1][1] + z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
    result.z = x * matrix.m[0][2] + y * matrix.m[1][2] + z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
    float w = x * matrix.m[0][3] + y * matrix.m[1][3] + z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
    assert(w != 0.0f);
    result.x /= w;
    result.y /= w;
    result.z /= w;
    return result;
}

Vector3 Vector3::TransformNormal(const Vector3& v, const Matrix4x4& matrix)
{
    return {
            v.x * matrix.m[0][0] + v.y * matrix.m[1][0] + v.z * matrix.m[2][0],
            v.x * matrix.m[0][1] + v.y * matrix.m[1][1] + v.z * matrix.m[2][1],
            v.x * matrix.m[0][2] + v.y * matrix.m[1][2] + v.z * matrix.m[2][2]
    };
}

float Vector3::AxisComponent(const Vector3& v, int axis)
{
    switch (axis) {
    case 0:  return v.x;
    case 1:  return v.y;
    default: return v.z;
    }
}

// 単項演算子オーバーロード
Vector3 Vector3::operator+() const {
    return *this;
}

Vector3 Vector3::operator-() const {
    return { -x, -y, -z };
}

// 二項演算子オーバーロード
Vector3 Vector3::operator+(const Vector3& v) const {
    return { x + v.x, y + v.y, z + v.z };
}

Vector3 Vector3::operator-(const Vector3& v) const {
    return { x - v.x, y - v.y, z - v.z };
}

Vector3 Vector3::operator*(float scalar) const {
    return { x * scalar, y * scalar, z * scalar };
}

// 複合代入演算子オーバーロード
Vector3& Vector3::operator+=(const Vector3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

// フレンド関数：スカラー倍の演算子オーバーロード
Vector3 operator*(float scalar, const Vector3& v) {
    return v * scalar;
}