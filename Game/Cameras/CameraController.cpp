#include "CameraController.h"
#include "WinApp.h"

void cCameraController::Initialize(sTransform* transform)
{
	assert(transform);

	transform_ = transform;
	worldMatrix_ = MakeAffineMatrix(transform_->scale, transform_->rotate, transform_->translate);
	Matrix4x4 viewMatrix = Inverse(worldMatrix_);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(cWinApp::kClientWidth) / float(cWinApp::kClientHeight), 0.1f, 100.0f);
	viewProjectionMatrix_ = Multiply(viewMatrix, projectionMatrix);
}

void cCameraController::Update()
{
	worldMatrix_ = MakeAffineMatrix(transform_->scale, transform_->rotate, transform_->translate);
	Matrix4x4 viewMatrix = Inverse(worldMatrix_);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(cWinApp::kClientWidth) / float(cWinApp::kClientHeight), 0.1f, 100.0f);
	viewProjectionMatrix_ = Multiply(viewMatrix, projectionMatrix);
}
