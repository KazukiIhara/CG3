#pragma once

#include "Lazieal.h"

class cCameraController {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(sTransform* transform);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ビュープロジェクションマトリックスを送る関数
	/// </summary>
	/// <returns></returns>
	Matrix4x4* GetViewProjectionMatrix() {
		return &viewProjectionMatrix_;
	}

	Matrix4x4* GetWorldMatrix() {
		return &worldMatrix_;
	}
private:

	/*カメラのトランスフォームを受け取る箱*/
	sTransform* transform_;

	/*カメラのワールドマトリックス*/
	Matrix4x4 worldMatrix_;

	/*ビュープロジェクションマトリックス*/
	Matrix4x4 viewProjectionMatrix_;

};

