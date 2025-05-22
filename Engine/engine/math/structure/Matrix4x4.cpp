#include "Matrix4x4.h"

#include <cmath>
#include <numbers>

#include "imgui.h"

#include "Vector3.h"
#include "Quaternion.h"

// コンストラクタ：すべての要素を0に初期化
Matrix4x4::Matrix4x4() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] = 0.0f;
        }
    }
}

void Matrix4x4::ImGuiMatrix()
{
    if (ImGui::BeginTable("MatrixTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        for (int j = 0; j < 4; ++j) {
            ImGui::TableNextRow();
            for (int i = 0; i < 4; ++i) {
                ImGui::TableSetColumnIndex(i);
                ImGui::Text("%.3f", m[j][i]);
            }
        }
        ImGui::EndTable();
    }
}

Matrix4x4 Matrix4x4::Transpose()
{
    Matrix4x4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = this->m[j][i];
        }
    }
    return result;
}

// 単位行列の生成
Matrix4x4 Matrix4x4::Identity() {
    Matrix4x4 result{};
    for (int i = 0; i < 4; ++i) {
        result.m[i][i] = 1.0f;
    }
    return result;
}

Matrix4x4 Matrix4x4::Multiply(const Matrix4x4& m1, const Matrix4x4& m2)
{
    Matrix4x4 result{};
    result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
    result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
    result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
    result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

    result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
    result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
    result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
    result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

    result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
    result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
    result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
    result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

    result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
    result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
    result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
    result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];

    return result;
}

// 拡縮行列の生成
Matrix4x4 Matrix4x4::Scale(const Vector3& scale) {
    Matrix4x4 result = Identity();
    result.m[0][0] = scale.x;
    result.m[1][1] = scale.y;
    result.m[2][2] = scale.z;
    return result;
}

// X軸回転行列
Matrix4x4 Matrix4x4::RotateX(float radian) {
    Matrix4x4 result = Identity();
    float cosTheta = std::cos(radian);
    float sinTheta = std::sin(radian);
    result.m[1][1] = cosTheta;
    result.m[1][2] = sinTheta;
    result.m[2][1] = -sinTheta;
    result.m[2][2] = cosTheta;
    return result;
}

// Y軸回転行列
Matrix4x4 Matrix4x4::RotateY(float radian) {
    Matrix4x4 result = Identity();
    float cosTheta = std::cos(radian);
    float sinTheta = std::sin(radian);
    result.m[0][0] = cosTheta;
    result.m[0][2] = -sinTheta;
    result.m[2][0] = sinTheta;
    result.m[2][2] = cosTheta;
    return result;
}

// Z軸回転行列
Matrix4x4 Matrix4x4::RotateZ(float radian) {
    Matrix4x4 result = Identity();
    float cosTheta = std::cos(radian);
    float sinTheta = std::sin(radian);
    result.m[0][0] = cosTheta;
    result.m[0][1] = sinTheta;
    result.m[1][0] = -sinTheta;
    result.m[1][1] = cosTheta;
    return result;
}

// 3軸の回転を合成した回転行列
Matrix4x4 Matrix4x4::Rotate(const Vector3& rotate) {
    return RotateX(rotate.x) * RotateY(rotate.y) * RotateZ(rotate.z);
}

// 移動行列の生成
Matrix4x4 Matrix4x4::Translate(const Vector3& translate) {
    Matrix4x4 result = Identity();
    result.m[3][0] = translate.x;
    result.m[3][1] = translate.y;
    result.m[3][2] = translate.z;
    return result;
}

Matrix4x4 Matrix4x4::NormalizeRotation(const Matrix4x4& matrix)
{
    Matrix4x4 result = matrix;
    result.m[0][0] = Vector3(matrix.m[0][0], matrix.m[1][0], matrix.m[2][0]).Length();
    result.m[1][1] = Vector3(matrix.m[0][1], matrix.m[1][1], matrix.m[2][1]).Length();
    result.m[2][2] = Vector3(matrix.m[0][2], matrix.m[1][2], matrix.m[2][2]).Length();
    return result;
}

// 行列の積
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
    Matrix4x4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

// LookAt行列の生成
Matrix4x4 Matrix4x4::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
    Vector3 z = target - eye;
    if (z.Length() < 1e-6f) {
        z = Vector3::ExprUnitZ;
    }
    Vector3 x = Vector3::Cross(up, z.Normalize());
    if (x.Length() < 1e-6f) {
        x = Vector3::ExprUnitX;
    }

    Vector3 y = Vector3::Cross(z.Normalize(), x.Normalize());

    Matrix4x4 result = Identity();
    result.m[0][0] = x.x; result.m[0][1] = y.x; result.m[0][2] = z.x; result.m[0][3] = 0.0f;
    result.m[1][0] = x.y; result.m[1][1] = y.y; result.m[1][2] = z.y; result.m[1][3] = 0.0f;
    result.m[2][0] = x.z; result.m[2][1] = y.z; result.m[2][2] = z.z; result.m[2][3] = 0.0f;
    result.m[3][0] = -Vector3::Dot(x, eye); 
    result.m[3][1] = -Vector3::Dot(y, eye);
    result.m[3][2] = -Vector3::Dot(z, eye);
    result.m[3][3] = 1.0f;
    return result;
}

Vector3 Matrix4x4::ExtractEulerAngles(const Matrix4x4& m)
{
    Vector3 euler{};

    // Y軸（ピッチ）を中心とした回転を考慮した XYZ 回転順の抽出
    if (std::abs(m.m[2][0]) < 1.0f) {
        // 一般ケース
        euler.y = std::asin(-m.m[2][0]); // Pitch（Y）

        float cosY = std::cos(euler.y);
        euler.x = std::atan2(m.m[2][1] / cosY, m.m[2][2] / cosY); // Roll（X）
        euler.z = std::atan2(m.m[1][0] / cosY, m.m[0][0] / cosY); // Yaw（Z）
    } else {
        // 万が一 Gimbal Lock の場合（Pitch ±90°）
        euler.y = (m.m[2][0] < 0) ? std::numbers::pi / 2.0f : -std::numbers::pi_v<float> / 2.0f;
        euler.x = 0.0f;
        euler.z = std::atan2(-m.m[0][1], m.m[1][1]);
    }

    return euler;
}

// アフィン変換行列の生成
Matrix4x4 Matrix4x4::Affine(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
    return  Scale(scale) * Rotate(rotate) * Translate(translate);
}

Matrix4x4 Matrix4x4::Affine(const Vector3& scale, const Quaternion& rotate, const Vector3& translate)
{
    return Scale(scale) * Quaternion::MakeRotateMatrix(rotate) * Translate(translate);
}

// 逆行列の生成
Matrix4x4 Matrix4x4::Inverse(const Matrix4x4& m) {
    Matrix4x4 result;
    float A;
    A = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
        - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
        - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
        + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
        + m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
        - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
        - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
        + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

    result.m[0][0] = 1 / A * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2]
        - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);
    result.m[0][1] = 1 / A * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2]
        + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
    result.m[0][2] = 1 / A * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2]
        - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
    result.m[0][3] = 1 / A * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2]
        + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

    result.m[1][0] = 1 / A * (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2]
        + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);
    result.m[1][1] = 1 / A * (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2]
        - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);
    result.m[1][2] = 1 / A * (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2]
        + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);
    result.m[1][3] = 1 / A * (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2]
        - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);

    result.m[2][0] = 1 / A * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1]
        - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);
    result.m[2][1] = 1 / A * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1]
        + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
    result.m[2][2] = 1 / A * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1]
        - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
    result.m[2][3] = 1 / A * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1]
        + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

    result.m[3][0] = 1 / A * (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1]
        + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);
    result.m[3][1] = 1 / A * (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1]
        - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);
    result.m[3][2] = 1 / A * (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1]
        + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);
    result.m[3][3] = 1 / A * (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1]
        - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);
    return result;
}

// 投資投影行列の生成
Matrix4x4 Matrix4x4::PerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
    Matrix4x4 result{};
    float tanHalfFovY = std::tan(fovY / 2.0f);
    result.m[0][0] = 1.0f / aspectRatio * (1.0f / tanHalfFovY);
    result.m[1][1] = 1.0f / tanHalfFovY;
    result.m[2][2] = farClip / (farClip - nearClip);
    result.m[2][3] = 1.0f;
    result.m[3][2] = -(nearClip * farClip) / (farClip - nearClip);
    return result;
}

// 正射影行列の生成
Matrix4x4 Matrix4x4::Orthographic(float left, float top, float right, float bottom, float nearClip, float farClip) {
    Matrix4x4 result = Identity();
    result.m[0][0] = 2.0f / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][2] = 1.0f / (farClip - nearClip);
    result.m[3][0] = (left + right) / (left - right);
    result.m[3][1] = (top + bottom) / (bottom - top);
    result.m[3][2] = nearClip / (nearClip - farClip);
    result.m[3][3] = 1.0f;
    return result;
}

Matrix4x4 Matrix4x4::MakeRotateAxisAngle(const Vector3& axis, float angle)
{
    // 回転軸を正規化
    Vector3 normalizeAxis = axis.Normalize();
    float x = normalizeAxis.x, y = normalizeAxis.y, z = normalizeAxis.z;

    // 角度の三角関数
    float cosTheta = std::cos(angle);
    float sinTheta = std::sin(angle);
    float oneMinusCosTheta = 1.0f - cosTheta;

    // 回転行列の各成分を計算
    Matrix4x4 rotation = Matrix4x4::Identity();
    rotation.m[0][0] = cosTheta + x * x * oneMinusCosTheta;
    rotation.m[0][1] = x * y * oneMinusCosTheta + z * sinTheta;
    rotation.m[0][2] = x * z * oneMinusCosTheta - y * sinTheta;
    rotation.m[0][3] = 0.0f;

    rotation.m[1][0] = y * x * oneMinusCosTheta - z * sinTheta;
    rotation.m[1][1] = cosTheta + y * y * oneMinusCosTheta;
    rotation.m[1][2] = y * z * oneMinusCosTheta + x * sinTheta;
    rotation.m[1][3] = 0.0f;

    rotation.m[2][0] = z * x * oneMinusCosTheta + y * sinTheta;
    rotation.m[2][1] = z * y * oneMinusCosTheta - x * sinTheta;
    rotation.m[2][2] = cosTheta + z * z * oneMinusCosTheta;
    rotation.m[2][3] = 0.0f;

    rotation.m[3][0] = 0.0f;
    rotation.m[3][1] = 0.0f;
    rotation.m[3][2] = 0.0f;
    rotation.m[3][3] = 1.0f;

    return rotation;
}

Matrix4x4 Matrix4x4::DirectionToDirection(const Vector3& from, const Vector3& to)
{
    // 入力ベクトルの正規化
    Vector3 fromNorm = from.Normalize();
    Vector3 toNorm = to.Normalize();

    // 回転軸を計算し、正規化
    Vector3 axis = Vector3::Cross(fromNorm, toNorm);

    // from と to が同じ方向の場合、単位行列を返す
    if (axis.Length() < 1e-6f) {
        // 同方向: 単位行列
        if (Vector3::Dot(fromNorm, toNorm) > 0.9999f) {
            return Matrix4x4::Identity();
        }

        // 逆方向: 任意の軸で180度回転
        Vector3 arbitraryAxis = (std::abs(fromNorm.x) < 0.1f) ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(0.0f, 1.0f, 0.0f);
        axis = Vector3::Cross(fromNorm, arbitraryAxis).Normalize();
        return MakeRotateAxisAngle(axis, static_cast<float>(std::numbers::pi));
    }

    axis = axis.Normalize();
    float cosTheta = Vector3::Dot(fromNorm, toNorm);

    // 回転行列を返す
    return MakeRotateAxisAngle(axis, std::acos(cosTheta));
}
