#pragma once
#include <cmath>
#include <cassert>

#include "Matrix4x4.h"

// Vector3 クラスの定義
class Vector3 
{
public:
    float x, y, z;

    // コンストラクタ
    Vector3(float x = 0, float y = 0, float z = 0);

    // 定数
    static const Vector3 ExprUnitX;
    static const Vector3 ExprUnitY;
    static const Vector3 ExprUnitZ;
    static const Vector3 ExprZero;

    // 加算
    static Vector3 Add(const Vector3& v1, const Vector3& v2);

    // 減算
    static Vector3 Subtract(const Vector3& v1, const Vector3& v2);

    // スカラー倍
    static Vector3 Multiply(float scalar, const Vector3& v);

    float Length()const;

    // 内積
    static float Dot(const Vector3& v1, const Vector3& v2);

    // 外積
    static Vector3 Cross(const Vector3& v1, const Vector3& v2);

    static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

    // 正規化
    Vector3 Normalize() const;

    // 行列による変換
    static Vector3 Transform(const Vector3& v, const Matrix4x4& matrix);
    Vector3 Transform(const Matrix4x4& matrix) const;

    static Vector3 TransformNormal(const Vector3& v, const Matrix4x4& matrix);
    Vector3 TransformNormal(const Matrix4x4& matrix)const;

    static float AxisComponent(const Vector3& v, int axis);

    // 単項演算子オーバーロード
    Vector3 operator+() const;
    Vector3 operator-() const;

    // 二項演算子オーバーロード
    Vector3 operator+(const Vector3& v) const;
    Vector3 operator-(const Vector3& v) const;
    Vector3 operator*(float scalar) const;

    // 複合代入演算子オーバーロード
    Vector3& operator+=(const Vector3& v);
    Vector3& operator-=(const Vector3& v);
    Vector3& operator*=(float scalar);

    // フレンド関数：スカラー倍の演算子オーバーロード
    friend Vector3 operator*(float scalar, const Vector3& v);
};