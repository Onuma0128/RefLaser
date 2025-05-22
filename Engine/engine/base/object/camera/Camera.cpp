#include "Camera.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG
#include "Input.h"

void Camera::Initialize()
{
	debugTransform_ = { {1.0f,1.0f,1.0f},{0.26f,0.0f,0.0f},{0.0f,4.0f,-15.0f} };
	transform_ = { {1.0f,1.0f,1.0f},{0.26f,0.0f,0.0f},{0.0f,4.0f,-15.0f} };
	fovY_ = 0.45f;
	aspectRatio_ = float(WinApp::kClientWidth) / float(WinApp::kClientHeight);
	nearClip_ = 0.1f;
	farClip_ = 100.0f;

	UpdateMatrix(transform_);
	isDebug_ = false;
}

void Camera::Update()
{
#ifdef _DEBUG
	if (isDebug_) {
		DebugCamera();
		return;
	}
#endif // _DEBUG
	NormalCamera();
}

void Camera::DebugCamera()
{
	const float moveSpeed = 0.25f;

	Vector3 defaultForward = Vector3::ExprUnitZ;
	Matrix4x4 rotationMatrix = Matrix4x4::RotateY(debugTransform_.rotation.y);
	Vector3 forward = defaultForward.Transform(rotationMatrix);

	// 右方向ベクトルを計算
	Vector3 defaultRight = Vector3::ExprUnitX;
	Vector3 right = defaultRight.Transform(rotationMatrix);

	Input* input = Input::GetInstance();
	if (input->PushKey(DIK_W)) {
		debugTransform_.translation += forward * moveSpeed;
	}
	if (input->PushKey(DIK_S)) {
		debugTransform_.translation -= forward * moveSpeed;
	}
	if (input->PushKey(DIK_A)) {
		debugTransform_.translation -= right * moveSpeed;
	}
	if (input->PushKey(DIK_D)) {
		debugTransform_.translation += right * moveSpeed;
	}
	if (input->PushKey(DIK_Q)) {
		debugTransform_.translation.y -= moveSpeed;
	}
	if (input->PushKey(DIK_E)) {
		debugTransform_.translation.y += moveSpeed;
	}

	// 右クリックされているなら
	if (input->PushMouseButton(1)) {
		// マウスによるカメラ回転
		float mouseDeltaX = static_cast<float>(input->GetMouseDeltaX());
		float mouseDeltaY = static_cast<float>(input->GetMouseDeltaY()) * -1.0f;

		// マウスの移動をカメラの回転に変換
		debugTransform_.rotation.y += mouseDeltaX * mouseSensitivity_;
		debugTransform_.rotation.x += mouseDeltaY * mouseSensitivity_;
	}

	UpdateMatrix(debugTransform_);
}

void Camera::NormalCamera()
{
	debugTransform_ = transform_;
	UpdateMatrix(transform_);
}

void Camera::SetLookAt(const Vector3& eye, const Vector3& target)
{
	// カメラの位置を設定
	transform_.translation = eye;

	// 方向ベクトル
	Vector3 forward = target - eye;
	if (forward.Length() < 1e-6f) {
		// 方向が0だと回転計算ができない
		return;
	}
	forward = forward.Normalize();

	// Y軸を上と仮定して右ベクトルを求める
	Vector3 up = { 0.0f, 1.0f, 0.0f };
	Vector3 right = Vector3::Cross(up, forward).Normalize();

	// 正確な up を再計算（直交性を保証）
	up = Vector3::Cross(forward, right);

	// 回転行列を構成（カメラのローカル座標系）
	Matrix4x4 rotMat{};

	// 1列目（X軸：Right）
	rotMat.m[0][0] = right.x;
	rotMat.m[1][0] = right.y;
	rotMat.m[2][0] = right.z;
	rotMat.m[3][0] = 0.0f;

	// 2列目（Y軸：Up）
	rotMat.m[0][1] = up.x;
	rotMat.m[1][1] = up.y;
	rotMat.m[2][1] = up.z;
	rotMat.m[3][1] = 0.0f;

	// 3列目（Z軸：Forward）
	rotMat.m[0][2] = forward.x;
	rotMat.m[1][2] = forward.y;
	rotMat.m[2][2] = forward.z;
	rotMat.m[3][2] = 0.0f;

	// 4列目（平行移動・今回は0）
	rotMat.m[0][3] = 0.0f;
	rotMat.m[1][3] = 0.0f;
	rotMat.m[2][3] = 0.0f;
	rotMat.m[3][3] = 1.0f;

	// 行列からオイラー角を抽出
	transform_.rotation = Matrix4x4::ExtractEulerAngles(rotMat);
}

void Camera::UpdateMatrix(EulerTransform transform)
{
	worldMatrix_ = Matrix4x4::Affine(transform.scale, transform.rotation, transform.translation);

	viewMatrix_ = Matrix4x4::Inverse(worldMatrix_);

	projectionMatrix_ = Matrix4x4::PerspectiveFov(fovY_, aspectRatio_, nearClip_, farClip_);
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

void Camera::CameraImGui()
{
#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::Checkbox("debug", &isDebug_);

	Vector3 rotation = transform_.rotation;
	Vector3 translation = transform_.translation;
	ImGui::DragFloat3("rotate", &rotation.x, 0.01f);
	ImGui::DragFloat3("translate", &translation.x, 0.01f);
	transform_.rotation = rotation;
	transform_.translation = translation;

	ImGui::End();
#endif // _DEBUG
}