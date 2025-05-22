#include "Input.h"
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "WinApp.h"

Input* Input::instance_ = nullptr;

Input* Input::GetInstance()
{
	if (instance_ == nullptr) {
		instance_ = new Input;
	}
	return instance_;
}

void Input::Finalize()
{
	delete instance_;
	instance_ = nullptr;
}

void Input::Initialize(WinApp* winApp)
{
	winApp_ = winApp;

	HRESULT hr{};

	// 前回のキー入力を保存
	memcpy(keyPre_, key_, sizeof(key_));

	hr = DirectInput8Create(
		winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));


	// キーボードデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	// 排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(
		winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));


	// マウスデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(hr));
	// 排他制御レベルのセット
	hr = mouse_->SetCooperativeLevel(
		winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	mouse_->Acquire();


	//// ジョイスティックデバイスの生成
	//hr = directInput_->CreateDevice(GUID_Joystick, &joystick_, NULL);
	//assert(SUCCEEDED(hr));
	//// 入力データ形式のセット
	//hr = joystick_->SetDataFormat(&c_dfDIJoystick2);
	//assert(SUCCEEDED(hr));
	//// 排他制御レベルのセット
	//hr = joystick_->SetCooperativeLevel(
	//	winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	//assert(SUCCEEDED(hr));

	//// ジョイスティックの取得開始
	//joystick_->Acquire();

	//// ジョイスティックの範囲設定の初期化
	//if (joystick_) {
	//	DIPROPRANGE dipr;
	//	dipr.diph.dwSize = sizeof(DIPROPRANGE);
	//	dipr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	//	dipr.diph.dwObj = DIJOFS_X; // X軸
	//	dipr.diph.dwHow = DIPH_BYOFFSET;
	//	dipr.lMin = -1000; // 最小値
	//	dipr.lMax = 1000;  // 最大値

	//	joystick_->SetProperty(DIPROP_RANGE, &dipr.diph);
	//	joystickMinX_ = dipr.lMin;
	//	joystickMaxX_ = dipr.lMax;

	//	// 同様にY軸にも適用
	//	dipr.diph.dwObj = DIJOFS_Y; // Y軸
	//	joystick_->SetProperty(DIPROP_RANGE, &dipr.diph);
	//	joystickMinY_ = dipr.lMin;
	//	joystickMaxY_ = dipr.lMax;
	//}
}

void Input::Update()
{
	// 前回のキー入力を保存
	memcpy(keyPre_, key_, sizeof(key_));
	// キーボード情報の取得開始
	keyboard_->Acquire();
	// 全キーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(key_), key_);

	// マウス情報の取得開始
	mouse_->Acquire();
	HRESULT hr = mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);
	if (FAILED(hr)) {
		// デバイスが一時的に使用できない場合は再取得
		mouse_->Acquire();
	}

	// マウスの移動量とボタンの状態を取得
	mouseDeltaX_ = mouseState_.lX;
	mouseDeltaY_ = mouseState_.lY;
	POINT cursorPos;
	if (GetCursorPos(&cursorPos)) {
		// クライアント座標系に変換
		ScreenToClient(winApp_->GetHwnd(), &cursorPos);
		mousePosX_ = cursorPos.x;
		mousePosY_ = cursorPos.y;
	}

	memcpy(mouseButtonPre_, mouseButton_, sizeof(mouseButton_));
	memcpy(mouseButton_, mouseState_.rgbButtons, sizeof(mouseButton_));

	// XInput の更新
	xInputStatePre_ = xInputState_; // 前回の状態を保存
	ZeroMemory(&xInputState_, sizeof(XINPUT_STATE));
	DWORD result = XInputGetState(0, &xInputState_); // コントローラー 0 を取得
	if (result != ERROR_SUCCESS) {
		// コントローラーが接続されていない場合
		ZeroMemory(&xInputState_, sizeof(XINPUT_STATE));
	}
}

bool Input::PushKey(BYTE keyNumber)
{
	// 指定キーが押していればtrueを返す
	return key_[keyNumber] != 0;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// 指定キーが現在押されていて、前回は押されていなかった場合にtrueを返す
	return key_[keyNumber] && !keyPre_[keyNumber];
}

bool Input::PushMouseButton(int buttonNumber) const
{
	return (mouseButton_[buttonNumber] & 0x80) != 0;
}

bool Input::TriggerMouseButton(int buttonNumber) const
{
	return (mouseButton_[buttonNumber] & 0x80) != 0 && !(mouseButtonPre_[buttonNumber] & 0x80);
}

bool Input::PushGamepadButton(WORD button) const
{
	return (xInputState_.Gamepad.wButtons & button) != 0;
}

bool Input::TriggerGamepadButton(WORD button) const
{
	return (xInputState_.Gamepad.wButtons & button) != 0 &&
		!(xInputStatePre_.Gamepad.wButtons & button);
}

float Input::NormalizeStickValue(SHORT value, SHORT deadzone)
{
	if (abs(value) < deadzone) {
		return 0.0f; // デッドゾーン内は 0
	}
	return static_cast<float>(value) / 32767.0f; // -1.0 ～ 1.0 に正規化
}

float Input::GetGamepadLeftStickX() const {
	return NormalizeStickValue(xInputState_.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
}

float Input::GetGamepadLeftStickY() const {
	return NormalizeStickValue(xInputState_.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
}

float Input::GetGamepadRightStickX() const {
	return NormalizeStickValue(xInputState_.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

float Input::GetGamepadRightStickY() const {
	return NormalizeStickValue(xInputState_.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

float Input::GetGamepadLeftTrigger() const
{
	return static_cast<float>(xInputState_.Gamepad.bLeftTrigger) / 255.0f;
}

float Input::GetGamepadRightTrigger() const
{
	return static_cast<float>(xInputState_.Gamepad.bRightTrigger) / 255.0f;
}
