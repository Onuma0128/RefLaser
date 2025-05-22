// Vector2.cpp
#include "Vector2.h"

const Vector2 Vector2::UnitX = { 1.0f, 0.0f };
const Vector2 Vector2::UnitY = { 0.0f, 1.0f };
const Vector2 Vector2::Zero = { 0.0f, 0.0f };

Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector2 Vector2::Add(const Vector2& v1, const Vector2& v2) {
    return { v1.x + v2.x, v1.y + v2.y };
}

Vector2 Vector2::Subtract(const Vector2& v1, const Vector2& v2) {
    return { v1.x - v2.x, v1.y - v2.y };
}

Vector2 Vector2::Multiply(float scalar, const Vector2& v) {
    return { v.x * scalar, v.y * scalar };
}

float Vector2::Length() const {
    return std::sqrt(x * x + y * y);
}

float Vector2::Dot(const Vector2& v1, const Vector2& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

Vector2 Vector2::Lerp(const Vector2& v1, const Vector2& v2, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return {
        v1.x + (v2.x - v1.x) * t,
        v1.y + (v2.y - v1.y) * t
    };
}

Vector2 Vector2::Normalize() const {
    float len = Length();
    assert(len != 0 && "Zero division during normalization.");
    return { x / len, y / len };
}

Vector2 Vector2::operator+() const {
    return *this;
}

Vector2 Vector2::operator-() const {
    return { -x, -y };
}

Vector2 Vector2::operator+(const Vector2& v) const {
    return { x + v.x, y + v.y };
}

Vector2 Vector2::operator-(const Vector2& v) const {
    return { x - v.x, y - v.y };
}

Vector2 Vector2::operator*(float scalar) const {
    return { x * scalar, y * scalar };
}

Vector2& Vector2::operator+=(const Vector2& v) {
    x += v.x;
    y += v.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

Vector2& Vector2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2 operator*(float scalar, const Vector2& v) {
    return v * scalar;
}
