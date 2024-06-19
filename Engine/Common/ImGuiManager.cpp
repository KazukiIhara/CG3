#include "ImGuiManager.h"
#include "WinApp.h"
#include "DirectXCommon.h"

cImGuiManager* cImGuiManager::GetInstance()
{
	static cImGuiManager instance;
	return &instance;
}

void cImGuiManager::Initialize()
{
	/*ImGuiの初期化*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(cWinApp::GetHwnd());
	ImGui_ImplDX12_Init(cDirectXCommon::GetDevice(),
		cDirectXCommon::GetSwapChainDesc().BufferCount,
		cDirectXCommon::GetRTVDesc().Format,
		cDirectXCommon::GetSRVDescriptorHeap(),
		cDirectXCommon::GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		cDirectXCommon::GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
	);
}

void cImGuiManager::BeginFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void cImGuiManager::EndFrame()
{
	/*ImGui内部コマンドの生成*/
	ImGui::End();
	ImGui::Render();
}

void cImGuiManager::PreDraw()
{
	/*描画用のDescriptorHeapの設定*/
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = { cDirectXCommon::GetSRVDescriptorHeap() };
	cDirectXCommon::GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());
}

void cImGuiManager::Draw()
{
	/*実際のCommandListのImGuiの描画コマンドを積む*/
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cDirectXCommon::GetCommandList());
}

void cImGuiManager::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
