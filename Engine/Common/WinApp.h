#pragma once
#include <Windows.h>
#include <cstdint>

class cWinApp
{
public: /*静的メンバ変数*/
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

public: /*静的メンバ関数*/


	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static cWinApp* GetInstance();

	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: /*メンバ関数*/

	/// <summary>
	/// ゲームウィンドウの作成
	/// </summary>
	void CreateGameWindow(const wchar_t* title = L"ZealazyForDirectXGame", UINT windowStyle = WS_OVERLAPPEDWINDOW,
		int32_t clientWidth = kClientWidth, int32_t clientHeight = kClientHeight);

	/// <summary>
	/// ゲームウィンドウの破棄
	/// </summary>
	void TerminateGameWindow();

	/// <summary>
	/// メッセージの処理
	/// </summary>
	/// <returns></returns>
	bool ProcessMessage();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	static HWND GetHwnd()
	{
		return GetInstance()->hwnd_;
	}

private: //メンバ変数
	/*Window関連*/
	HWND hwnd_ = nullptr; /*ウィンドウハンドル*/
	WNDCLASS wc_{};
};

