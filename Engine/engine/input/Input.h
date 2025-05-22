#pragma once
#include <wrl.h>
#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <cassert>
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "Xinput.lib")

class WinApp;

class Input
{
private:
	static Input* instance_;

	Input() = default;
	~Input() = default;
	Input(Input&) = delete;
	Input& operator=(Input&) = delete;

public:
	// シングルトンインスタンスの取得
	static Input* GetInstance();
public:

	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	// 初期化
	void Initialize(WinApp* winApp);
	// 更新
	void Update();
	// 終了
	void Finalize();

	// キーの押下をチェック
	bool PushKey(BYTE keyNumber);
	// キーのトリガーをチェック
	bool TriggerKey(BYTE keyNumber);

	// マウスボタンの押下
	bool PushMouseButton(int buttonNumber)const;
	// マウスボタンのトリガーをチェック
	bool TriggerMouseButton(int buttonNumber)const;
	// マウスの移動量の取得
	int GetMouseDeltaX()const { return mouseDeltaX_; }
	int GetMouseDeltaY()const { return mouseDeltaY_; }
	// マウスの座標の取得
	int GetMousePosX()const { return mousePosX_; }
	int GetMousePosY()const { return mousePosY_; }

	// ジョイスティックボタンの押下チェック
	bool PushGamepadButton(WORD button) const;
	// ジョイスティックボタンのトリガーチェック
	bool TriggerGamepadButton(WORD button) const;
	// スティックのX軸とY軸の取得
	// 左
	float GetGamepadLeftStickX() const;
	float GetGamepadLeftStickY() const;
	// 右
	float GetGamepadRightStickX() const;
	float GetGamepadRightStickY() const;
	// LT/RT
	float GetGamepadLeftTrigger() const;
	float GetGamepadRightTrigger() const;

private:
	// スティックの正規化
	static float NormalizeStickValue(SHORT value, SHORT deadzone);

private:
	// WindowsAPI
	WinApp* winApp_ = nullptr;
	// 入力デバイス
	ComPtr<IDirectInput8> directInput_;
	// キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard_;
	// マウスのデバイス
	ComPtr<IDirectInputDevice8> mouse_;
	// ジョイスティックのデバイス
	ComPtr<IDirectInputDevice8> joystick_;

	/*==================== キーボード ====================*/

	// 全キーの状態
	BYTE key_[256] = {};
	// 前回の全キーの状態
	BYTE keyPre_[256] = {};

	/*==================== マウス ====================*/

	// マウスの状態
	DIMOUSESTATE mouseState_ = {};
	// マウスボタンの状態
	BYTE mouseButton_[4] = {};
	// マウスボタンの前回の状態
	BYTE mouseButtonPre_[4] = {};
	// マウスの移動量
	int mouseDeltaX_ = 0;
	int mouseDeltaY_ = 0;
	// マウスの座標
	int mousePosX_ = 0;
	int mousePosY_ = 0;

	/*==================== ジョイスティック ====================*/

	// ジョイスティックの状態
	XINPUT_STATE xInputState_ = {};
	// 前回のジョイスティックの状態
	XINPUT_STATE xInputStatePre_ = {};
	// ジョイスティックの範囲情報
	LONG joystickMinX_ = -1000;
	LONG joystickMaxX_ = 1000;
	LONG joystickMinY_ = -1000;
	LONG joystickMaxY_ = 1000;

};