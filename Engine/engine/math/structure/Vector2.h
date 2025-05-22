// Vector2.h
#pragma once
#include <cmath>
#include <cassert>
#include <algorithm>

class Vector2 {
public:
    float x, y;

    Vector2(float x = 0.0f, float y = 0.0f);

    static const Vector2 UnitX;
    static const Vector2 UnitY;
    static const Vector2 Zero;

    static Vector2 Add(const Vector2& v1, const Vector2& v2);
    static Vector2 Subtract(const Vector2& v1, const Vector2& v2);
    static Vector2 Multiply(float scalar, const Vector2& v);
    static float Dot(const Vector2& v1, const Vector2& v2);
    static Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);

    float Length() const;
    Vector2 Normalize() const;

    Vector2 operator+() const;
    Vector2 operator-() const;
    Vector2 operator+(const Vector2& v) const;
    Vector2 operator-(const Vector2& v) const;
    Vector2 operator*(float scalar) const;

    Vector2& operator+=(const Vector2& v);
    Vector2& operator-=(const Vector2& v);
    Vector2& operator*=(float scalar);

    friend Vector2 operator*(float scalar, const Vector2& v);
};
