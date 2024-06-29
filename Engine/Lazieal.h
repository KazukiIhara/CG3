#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <cassert>
#include <numbers>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"


/*---構造体---*/

struct Matrix2x2 {
	float m[2][2];
};

struct Matrix3x3 {
	float m[3][3];
};

struct sTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct sVertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material {
	Vector4 color;
	int32_t enbleLighting;
	float padding[3];
	Matrix4x4 uvTransformMatrix;
	float shininess;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTransepose;
};

struct DirectionalLight {
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度
};

struct MaterialData {
	std::string textureFilePath;
	uint32_t textureHandle;
	Vector4 color;
	bool enbleLighting;
};


struct ModelData {
	std::vector<sVertexData> vertices;
	MaterialData material;
};

struct CameraForGPU {
	Vector3 worldPosition;
};


/*---関数---*/

int Add(const int& a, const int& b);
int Subtract(const int& a, const int& b);
int Multiply(const int& a, const int& b);
int Divide(const int& a, const int& b);

float Add(const float& a, const float& b);
float Subtract(const float& a, const float& b);
float Multiply(const float& a, const float& b);
float Divide(const float& a, const float& b);

Vector2 Add(const Vector2& a, const Vector2& b);
Vector2 Subtract(const Vector2& a, const Vector2& b);
Vector2 Multiply(const Vector2& a, const Vector2& b);
Vector2 Divide(const Vector2& a, const Vector2& b);

Vector3 Add(const Vector3& a, const Vector3& b);
Vector3 Subtract(const Vector3& a, const Vector3& b);
Vector3 Multiply(const Vector3& a, const Vector3& b);
Vector3 Multiply(const float& scalar, const Vector3& a);//3次元ベクトルのスカラー倍関数
Vector3 Divide(const Vector3& a, const Vector3& b);
float Dot(Vector3 a, Vector3 b);//3次元ベクトルの内積を返す関数
float Length(Vector3 a);//3次元ベクトルの長さをfloat型で返す関数
Vector3 Normalize(Vector3 a);//正規化した3次元ベクトルを返す関数

Matrix4x4 Add(const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 Subtract(const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 Multiply(const float& scalar, const Matrix4x4& a);
Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 Inverse(const Matrix4x4& a);
Matrix4x4 Transpose(const Matrix4x4& a);
Matrix4x4 MakeIdentity4x4();

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Vector3 Transform(const  Vector3& vector, const  Matrix4x4& matrix);

Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 MakeRotateZMatrixRad(float radian);

Matrix4x4 MakeRotateXYZMatrix(Vector3 rotate);
Matrix4x4 MakeRotateXYZMatrixRad(Vector3 rotate);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRaito, float nearClip, float farClip);
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
