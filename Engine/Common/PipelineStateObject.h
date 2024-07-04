#pragma once
#include "DirectXCommon.h"
#include "wrl.h"
#include <dxcapi.h>
#include <string>

class cPipelineStateObject {
public:

	// ブレンドモード
	enum Blendmode {
		kBlendModeNone,
		kBlendModeNormal,
		kBlendModeAdd,
		kBlendModeSubtract,
		kBlendModeMultiply,
		kBlendModeScreen,
	};

	// インスタンスの取得
	static cPipelineStateObject* GetInstance();

	// 初期化
	void Initialize();

	// ルートシグネイチャ

	// Primitive用ルートシグネイチャのゲッター
	static ID3D12RootSignature* GetPrimitive3dRootSignature() {
		return GetInstance()->primitiveRootSignature_.Get();
	}

	// 3Dモデル用ルートシグネイチャのゲッター
	static ID3D12RootSignature* Get3DModelRootSignature() {
		return GetInstance()->model3DRootSignature_.Get();
	}

	// Particle用ルートシグネイチャのゲッター
	static ID3D12RootSignature* GetParticleRootSignature() {
		return GetInstance()->particleRootSignature_.Get();
	}


	// PSO
	static ID3D12PipelineState* GetPrimitive3dPipelineState(Blendmode blendMode);

	// 3Dモデル用のPSOのゲッター
	static ID3D12PipelineState* Get3DModelPipelineState(Blendmode blendMode);

	// Particle用PSOのゲッター
	static ID3D12PipelineState* GetParticlePipelineState(Blendmode blendMode);

private:
	cPipelineStateObject() = default;
	~cPipelineStateObject() = default;
	cPipelineStateObject(const cPipelineStateObject&) = delete;
	const cPipelineStateObject& operator=(const cPipelineStateObject&) = delete;

	// RootSignatureの作成
	// Primitve3d
	void CreatePrimitive3dRootSignature();
	// 3Dmodel用
	void Create3DModelRootSignature();
	// Particle用
	void CreateParticleRootSignature();

	// InputLayoutの設定
	D3D12_INPUT_LAYOUT_DESC InputLayoutSetting();

	// BlendStateの設定
	D3D12_BLEND_DESC BlendStateSetting(uint32_t blendModeNum);

	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC RasterizerStateSetting();

	// DepthStencilStateの設定
	// 3DModel
	D3D12_DEPTH_STENCIL_DESC DepthStecilDescSetting3DModel();
	// Particle
	D3D12_DEPTH_STENCIL_DESC DepthStecilDescSettingParticle();

	// Shaderをコンパイルする関数をまとめている関数
	void CompileShaders();

	// PSOの作成
	// Primitive3d用
	void CreatePrimitivePipelineStateObject();
	// 3Dmodel用
	void Create3DModelPipelineStateObject();
	// Particle用
	void CreateParticlePipelineStateObject();

	// シェーダーをコンパイルする関数
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

	// ルートシグネイチャ
	// Primitive用
	Microsoft::WRL::ComPtr<ID3D12RootSignature> primitiveRootSignature_;
	// 3Dmodel用
	Microsoft::WRL::ComPtr<ID3D12RootSignature> model3DRootSignature_;
	// Particle用
	Microsoft::WRL::ComPtr<ID3D12RootSignature> particleRootSignature_;

	// 頂点シェーダの塊
	// Primitive3d用
	Microsoft::WRL::ComPtr<ID3DBlob> primitiveVertexShaderBlob_;
	// 3Dmodel用
	Microsoft::WRL::ComPtr<ID3DBlob> model3DVertexShaderBlob_;
	// Particle用
	Microsoft::WRL::ComPtr<ID3DBlob> particleVertexShaderBlob_;

	// 色情報データの塊
	Microsoft::WRL::ComPtr<ID3DBlob> primitivePixelShaderBlob_;
	// 3Dmodel用
	Microsoft::WRL::ComPtr<ID3DBlob> model3DPixelShaderBlob_;
	// Particle用
	Microsoft::WRL::ComPtr<ID3DBlob> particlePixelShaderBlob_;

	// グラフィックスパイプラインステイトオブジェクト a.k.a PSO (本人)
	// Primitive3d用
	Microsoft::WRL::ComPtr <ID3D12PipelineState> primitiveGraphicsPipelineState_[kBlendModeNum];
	// 3Dmodel用
	Microsoft::WRL::ComPtr <ID3D12PipelineState> model3DGraphicsPipelineState_[kBlendModeNum];
	// Particle用
	Microsoft::WRL::ComPtr <ID3D12PipelineState> particleGraphicsPipelineState_[kBlendModeNum];
};
