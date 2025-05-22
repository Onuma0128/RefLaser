#pragma once
#include <windows.h>
#include <cstdint>

class WinApp
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// メッセージの処理
	bool ProcessMessage();
	// 終了
	void Finalize();
	//Getter
	HWND GetHwnd()const { return hwnd_; }
	HINSTANCE GetHInstance()const { return wc_.hInstance; }

private:
	// ウィンドウハンドル
	HWND hwnd_ = nullptr;
	// ウィンドウクラスの設定
	WNDCLASS wc_{};
};

