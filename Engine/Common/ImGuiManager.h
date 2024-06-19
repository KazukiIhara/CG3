#pragma once
/*ImGui*/
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include <Windows.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
);

class cImGuiManager
{
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static cImGuiManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理先頭の処理
	/// </summary>
	void BeginFrame();


	/// <summary>
	/// 更新処理最後の処理
	/// </summary>
	void EndFrame();

	/// <summary>
	/// 描画前準備
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ImGuiの終了処理
	/// </summary>
	void Finalize();

private:
	/*こぴーきんし*/
	cImGuiManager() = default;
	~cImGuiManager() = default;
	cImGuiManager(const cImGuiManager&) = delete;
	const cImGuiManager& operator=(const cImGuiManager&) = delete;
};

