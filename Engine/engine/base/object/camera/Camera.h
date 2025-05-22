#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <wrl.h>

#include "WinApp.h"

#include "Transform.h"
#include "Matrix4x4.h"

using Microsoft::WRL::ComPtr;

class Camera
{
public:
	void Initialize();

	void Update();

	void UpdateMatrix(EulerTransform transform);

	void CameraImGui();

private:

	void DebugCamera();
	void NormalCamera();

	/*==================== アクセッサ ====================*/

public:
	// setter
	// RT部分
	void SetRotation(const Vector3& rotate) { transform_.rotation = rotate; }
	void SetTranslation(const Vector3& translate) { transform_.translation = translate; }

	// カメラの設定
	void SetFovY(const float fovY) { fovY_ = fovY; }
	void SetAspectRatio(const float aspectRatio) { aspectRatio_ = aspectRatio; }
	void SetNearClip(const float nearClip) { nearClip_ = nearClip; }
	void SetFarClip(const float farClip) { farClip_ = farClip; }
	void SetLookAt(const Vector3& eye, const Vector3& target);

	// getter
	const Matrix4x4& GetWorldMatrix()const { return worldMatrix_; }
	const Matrix4x4& GetViewMatrix()const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix()const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix()const { return viewProjectionMatrix_; }
	const Matrix4x4& GetViewportMatrix() {
		viewportMatrix_ = Matrix4x4::Identity();
		viewportMatrix_.m[0][0] = static_cast<float>(WinApp::kClientWidth) / 2.0f;
		viewportMatrix_.m[1][1] = static_cast<float>(WinApp::kClientHeight) / 2.0f;
		viewportMatrix_.m[3][0] = static_cast<float>(WinApp::kClientWidth) / 2.0f;
		viewportMatrix_.m[3][1] = static_cast<float>(WinApp::kClientHeight) / 2.0f;
		return viewportMatrix_;
	}

	const Vector3& GetRotation()const { return transform_.rotation; }
	const Vector3& GetTranslation()const { return transform_.translation; }
	const bool GetIsDebug()const { return isDebug_; }

	const float GetNearClip()const { return nearClip_; }
	const float GetFarClip()const { return farClip_; }

private:

	/*==================== カメラの変数 ====================*/

	EulerTransform transform_;
	EulerTransform debugTransform_;

	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 viewProjectionMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewportMatrix_;

	float fovY_;
	float aspectRatio_;
	float nearClip_;
	float farClip_;

	float mouseSensitivity_ = 0.001f;
	bool isDebug_ = false;
};