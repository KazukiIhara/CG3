#include "Lazieal.h"

/*---関数定義---*/

int Add(const int& a, const int& b) {
	return a + b;
}

int Subtract(const int& a, const int& b) {
	return a - b;
}

int Multiply(const int& a, const int& b) {
	return a * b;
}

int Divide(const int& a, const int& b) {
	return a / b;
}


float Add(const float& a, const float& b) {
	return a + b;
}

float Subtract(const float& a, const float& b) {
	return a - b;
}

float Multiply(const float& a, const float& b) {
	return a * b;
}

float Divide(const float& a, const float& b) {
	return a / b;
}


Vector2 Add(const Vector2& a, const Vector2& b) {
	return{ a.x + b.x,a.y + b.y };
}

Vector2 Subtract(const Vector2& a, const Vector2& b) {
	return{ a.x - b.x,a.y - b.y };
}

Vector2 Multiply(const Vector2& a, const Vector2& b) {
	return{ a.x * b.x,a.y * b.y };
}

Vector2 Divide(const Vector2& a, const Vector2& b) {
	return{ a.x / b.x,a.y / b.y };
}


Vector3 Add(const Vector3& a, const Vector3& b) {
	return{ a.x + b.x,a.y + b.y,a.z + b.z };
}

Vector3 Subtract(const Vector3& a, const Vector3& b) {
	return{ a.x - b.x,a.y - b.y,a.z - b.z };
}

Vector3 Multiply(const Vector3& a, const Vector3& b) {
	return{ a.x * b.x,a.y * b.y,a.z * b.z };
}

Vector3 Multiply(const float& scalar, const Vector3& a) {
	return{ a.x * scalar,a.y * scalar,a.z * scalar };
}

Vector3 Divide(const Vector3& a, const Vector3& b) {
	return{ a.x / b.x,a.y / b.y,a.z / b.z };
}

float Dot(Vector3 a, Vector3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Length(Vector3 a) {
	return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

Vector3 Normalize(Vector3 a) {
	float length = Length(a);
	Vector3 normalizedVector;

	if (length != 0) {
		normalizedVector.x = a.x / length;
		normalizedVector.y = a.y / length;
		normalizedVector.z = a.z / length;
	} else {
		normalizedVector.x = 0;
		normalizedVector.y = 0;
		normalizedVector.z = 0;
	}

	return normalizedVector;
}


Matrix4x4 Add(const Matrix4x4& a, const Matrix4x4& b) {
	return
	{
		a.m[0][0] + b.m[0][0] ,a.m[0][1] + b.m[0][1] ,a.m[0][2] + b.m[0][2] ,a.m[0][3] + b.m[0][3] ,
		a.m[1][0] + b.m[1][0] ,a.m[1][1] + b.m[1][1] ,a.m[1][2] + b.m[1][2] ,a.m[1][3] + b.m[1][3] ,
		a.m[2][0] + b.m[2][0] ,a.m[2][1] + b.m[2][1] ,a.m[2][2] + b.m[2][2] ,a.m[2][3] + b.m[2][3] ,
		a.m[3][0] + b.m[3][0] ,a.m[3][1] + b.m[3][1] ,a.m[3][2] + b.m[3][2] ,a.m[3][3] + b.m[3][3] ,
	};
}

Matrix4x4 Subtract(const Matrix4x4& a, const Matrix4x4& b) {
	return
	{
		a.m[0][0] - b.m[0][0] ,a.m[0][1] - b.m[0][1] ,a.m[0][2] - b.m[0][2] ,a.m[0][3] - b.m[0][3] ,
		a.m[1][0] - b.m[1][0] ,a.m[1][1] - b.m[1][1] ,a.m[1][2] - b.m[1][2] ,a.m[1][3] - b.m[1][3] ,
		a.m[2][0] - b.m[2][0] ,a.m[2][1] - b.m[2][1] ,a.m[2][2] - b.m[2][2] ,a.m[2][3] - b.m[2][3] ,
		a.m[3][0] - b.m[3][0] ,a.m[3][1] - b.m[3][1] ,a.m[3][2] - b.m[3][2] ,a.m[3][3] - b.m[3][3] ,
	};
}

Matrix4x4 Multiply(const float& scalar, const Matrix4x4& a) {
	return
	{
		a.m[0][0] * scalar ,a.m[0][1] * scalar ,a.m[0][2] * scalar ,a.m[0][3] * scalar ,
		a.m[1][0] * scalar ,a.m[1][1] * scalar ,a.m[1][2] * scalar ,a.m[1][3] * scalar ,
		a.m[2][0] * scalar ,a.m[2][1] * scalar ,a.m[2][2] * scalar ,a.m[2][3] * scalar ,
		a.m[3][0] * scalar ,a.m[3][1] * scalar ,a.m[3][2] * scalar ,a.m[3][3] * scalar ,
	};
}

Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b) {
	return
	{
		a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0] + a.m[0][3] * b.m[3][0],		a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1] + a.m[0][3] * b.m[3][1],		a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][2],		a.m[0][0] * b.m[0][3] + a.m[0][1] * b.m[1][3] + a.m[0][2] * b.m[2][3] + a.m[0][3] * b.m[3][3],
		a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0] + a.m[1][3] * b.m[3][0],		a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1] + a.m[1][3] * b.m[3][1],		a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][2],		a.m[1][0] * b.m[0][3] + a.m[1][1] * b.m[1][3] + a.m[1][2] * b.m[2][3] + a.m[1][3] * b.m[3][3],
		a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0] + a.m[2][3] * b.m[3][0],		a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1] + a.m[2][3] * b.m[3][1],		a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][2],		a.m[2][0] * b.m[0][3] + a.m[2][1] * b.m[1][3] + a.m[2][2] * b.m[2][3] + a.m[2][3] * b.m[3][3],
		a.m[3][0] * b.m[0][0] + a.m[3][1] * b.m[1][0] + a.m[3][2] * b.m[2][0] + a.m[3][3] * b.m[3][0],		a.m[3][0] * b.m[0][1] + a.m[3][1] * b.m[1][1] + a.m[3][2] * b.m[2][1] + a.m[3][3] * b.m[3][1],		a.m[3][0] * b.m[0][2] + a.m[3][1] * b.m[1][2] + a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][2],		a.m[3][0] * b.m[0][3] + a.m[3][1] * b.m[1][3] + a.m[3][2] * b.m[2][3] + a.m[3][3] * b.m[3][3],

	};
}

Matrix4x4 Inverse(const Matrix4x4& a) {
	Matrix4x4 A = a;
	Matrix4x4 B = MakeIdentity4x4();

	int i, j, k;
	for (i = 0; i < 4; ++i) {
		float scale = 1.0f / A.m[i][i];
		for (j = 0; j < 4; ++j) {
			A.m[i][j] *= scale;
			B.m[i][j] *= scale;
		}
		for (k = 0; k < 4; ++k) {
			if (k != i) {
				float factor = A.m[k][i];
				for (j = 0; j < 4; ++j) {
					A.m[k][j] -= factor * A.m[i][j];
					B.m[k][j] -= factor * B.m[i][j];
				}
			}
		}
	}

	return B;
}

Matrix4x4 Transpose(const Matrix4x4& a) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = a.m[j][i];
		}
	}
	return result;
}

Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 identity{};
	identity.m[0][0] = 1.0f; identity.m[0][1] = 0.0f; identity.m[0][2] = 0.0f; identity.m[0][3] = 0.0f;
	identity.m[1][0] = 0.0f; identity.m[1][1] = 1.0f; identity.m[1][2] = 0.0f; identity.m[1][3] = 0.0f;
	identity.m[2][0] = 0.0f; identity.m[2][1] = 0.0f; identity.m[2][2] = 1.0f; identity.m[2][3] = 0.0f;
	identity.m[3][0] = 0.0f; identity.m[3][1] = 0.0f; identity.m[3][2] = 0.0f; identity.m[3][3] = 1.0f;
	return identity;
}

Matrix4x4 MakeInverseTransposeMatrix(const Matrix4x4& a) {
	Matrix4x4 result = Inverse(a);
	result = Transpose(result);
	return result;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result = {
	   { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f ,
		0.0f, 0.0f, 1.0f, 0.0f ,
	   translate.x,translate.y, translate.z, 1.0f}
	};
	return result;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result = {
		{scale.x, 0.0f, 0.0f, 0.0f,
		 0.0f, scale.y, 0.0f, 0.0f,
		 0.0f, 0.0f, scale.z, 0.0f,
		 0.0f, 0.0f, 0.0f, 1.0f,
		}
	};
	return result;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};
	Vector4 coord = { vector.x,vector.y,vector.z, 1.0f };
	Vector4 temp{};
	temp.x = coord.x * matrix.m[0][0] + coord.y * matrix.m[1][0] + coord.z * matrix.m[2][0] + coord.w * matrix.m[3][0];
	temp.y = coord.x * matrix.m[0][1] + coord.y * matrix.m[1][1] + coord.z * matrix.m[2][1] + coord.w * matrix.m[3][1];
	temp.z = coord.x * matrix.m[0][2] + coord.y * matrix.m[1][2] + coord.z * matrix.m[2][2] + coord.w * matrix.m[3][2];
	temp.w = coord.x * matrix.m[0][3] + coord.y * matrix.m[1][3] + coord.z * matrix.m[2][3] + coord.w * matrix.m[3][3];

	result.x = temp.x / temp.w;
	result.y = temp.y / temp.w;
	result.z = temp.z / temp.w;

	return result;

}

Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 rotateX{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,std::cos(radian),std::sin(radian),0.0f,
		0.0f,-std::sin(radian),std::cos(radian),0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	return rotateX;
}

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 rotateY{
		std::cos(radian),0.0f,-std::sin(radian),0.0f,
		0.0f,1.0f,0.0f,0.0f,
		std::sin(radian),0.0f,std::cos(radian),0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	return rotateY;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 rotateZ{
		std::cos(radian),std::sin(radian),0.0f,0.0f,
		-std::sin(radian),std::cos(radian),0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	return rotateZ;
}

Matrix4x4 MakeRotateZMatrixRad(float radian) {
	Matrix4x4 rotateZ{
		std::cos(radian),std::sin(radian),0.0f,0.0f,
		-std::sin(radian),std::cos(radian),0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	rotateZ = Multiply(std::numbers::pi_v<float>, rotateZ);

	return rotateZ;
}

Matrix4x4 MakeRotateXYZMatrix(Vector3 rotate) {
	Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateY = MakeRotateXMatrix(rotate.y);
	Matrix4x4 rotateZ = MakeRotateXMatrix(rotate.z);

	Matrix4x4 result = Multiply(rotateX, Multiply(rotateY, rotateZ));
	return result;
}

Matrix4x4 MakeRotateXYZMatrixRad(Vector3 rotate) {
	Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x * std::numbers::pi_v<float>);
	Matrix4x4 rotateY = MakeRotateYMatrix(rotate.y * std::numbers::pi_v<float>);
	Matrix4x4 rotateZ = MakeRotateZMatrix(rotate.z * std::numbers::pi_v<float>);

	Matrix4x4 result = Multiply(rotateX, Multiply(rotateY, rotateZ));
	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 result = Multiply(MakeScaleMatrix(scale), Multiply(MakeRotateXYZMatrixRad(rotate), MakeTranslateMatrix(translate)));
	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRaito, float nearClip, float farClip) {
	Matrix4x4 result =
	{
		(1.0f / aspectRaito) * (1.0f / std::tan(fovY / 2.0f)),0.0f,0.0f,0.0f,
		0.0f, 1.0f / std::tan(fovY / 2.0f), 0.0f, 0.0f,
		0.0f, 0.0f, farClip / (farClip - nearClip),1.0f,
		0.0f, 0.0f, (-nearClip * farClip) / (farClip - nearClip),0.0f,
	};
	return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result =
	{
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / (farClip - nearClip), 0.0f,
		(left + right) / (left - right),(top + bottom) / (bottom - top), nearClip / (nearClip - farClip),1.0f,
	};
	return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result =
	{
		width / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -height / 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, maxDepth - minDepth, 0.0f,
		left + width / 2.0f, top + height / 2.0f, minDepth, 1.0f,
	};
	return result;
}
