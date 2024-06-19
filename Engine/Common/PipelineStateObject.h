#pragma once
#include "DirectXCommon.h"
#include "wrl.h"
#include <dxcapi.h>
#include <string>

class cPipelineStateObject
{
public:

	// ブレンドモード
	enum Blendmode
	{
		kBlendModeNone,
		kBlendModeNormal,
		kBlendModeAdd,
		kBlendModeSubtract,
		kBlendModeMultiply,
		kBlendModeScreen,
	};

	static cPipelineStateObject* GetInstance();

	void Initialize();

	static ID3D12RootSignature* GetRootSignature()
	{
		return GetInstance()->rootSignature_.Get();
	}

	static ID3D12PipelineState* GetPipelineState(Blendmode blendMode);


private:
	cPipelineStateObject() = default;
	~cPipelineStateObject() = default;
	cPipelineStateObject(const cPipelineStateObject&) = delete;
	const cPipelineStateObject& operator=(const cPipelineStateObject&) = delete;

	/*RootSignatureの作成*/
	void CreateRootSignature();

	/*InputLayoutの設定*/
	D3D12_INPUT_LAYOUT_DESC InputLayoutSetting();

	/*BlendStateの設定*/
	D3D12_BLEND_DESC BlendStateSetting(uint32_t blendModeNum);

	/*RasterizerStateの設定*/
	D3D12_RASTERIZER_DESC RasterizerStateSetting();

	/*DepthStencilStateの設定*/
	D3D12_DEPTH_STENCIL_DESC DepthStecilDescSetting();

	/*Shaderをコンパイル*/
	void CompileShaders();

	/*PSOの作成*/
	void CreatePipelineStateObject();


	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		//CompileするShaderファイルへのパス
		const std::wstring& filePath,
		//Compileに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

private:

	// ブレンドモードの数
	static const uint32_t kBlendModeNum = 6;

	/*ルートシグネイチャ*/
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	/*頂点シェーダの塊*/
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob_;
	/*色情報データの塊*/
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob_;

	/*グラフィックスパイプラインステイトオブジェクト a.k.a PSO (本人)*/
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_ [kBlendModeNum];

};
