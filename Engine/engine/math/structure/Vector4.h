#pragma once

class Vector4 {
public:

	float x, y, z, w;

	Vector4(float x = 0, float y = 0, float z = 0, float w = 0);

    // 単項演算子オーバーロード
    Vector4 operator-() const;

    // 二項演算子オーバーロード
    Vector4 operator+(const Vector4& v) const;
    Vector4 operator-(const Vector4& v) const;
    Vector4 operator*(float scalar) const;

    // 複合代入演算子オーバーロード
    Vector4& operator+=(const Vector4& v);
    Vector4& operator-=(const Vector4& v);
    Vector4& operator*=(float scalar);
};