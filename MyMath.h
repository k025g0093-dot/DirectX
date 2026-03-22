#pragma once
#include "KamataEngine.h"
#include <cmath>


	// --- 画像にある青色の関数名 ---
// Vector3 と Matrix4x4 の掛け算 (座標変換)
static KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix) {
	KamataEngine::Vector3 result;

	// 行列計算 (x, y, z, w=1.0f として計算)
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	// w成分で割って通常の座標系に戻す (同次座標系から3次元へ)
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}
	// 行列の掛け算
	static KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2) {
		KamataEngine::Matrix4x4 result;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result.m[i][j] = 0;
				for (int k = 0; k < 4; k++) {
					result.m[i][j] += m1.m[i][k] * m2.m[k][j];
				}
			}
		}
		return result;
	}

	// Z軸回転行列
	static KamataEngine::Matrix4x4 MakeRotateZ(float angle) {
		float sin = std::sin(angle);
		float cos = std::cos(angle);
		KamataEngine::Matrix4x4 result;
		result.m[0][0] = cos;
		result.m[0][1] = sin;
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;
		result.m[1][0] = -sin;
		result.m[1][1] = cos;
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;
		result.m[2][0] = 0.0f;
		result.m[2][1] = 0.0f;
		result.m[2][2] = 1.0f;
		result.m[2][3] = 0.0f;
		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = 0.0f;
		result.m[3][3] = 1.0f;
		return result;
	}

	// 平行移動行列
	static KamataEngine::Matrix4x4 MakeTranslate(const KamataEngine::Vector3& translate) {
		KamataEngine::Matrix4x4 result = {};
		result.m[0][0] = 1.0f;
		result.m[1][1] = 1.0f;
		result.m[2][2] = 1.0f;
		result.m[3][0] = translate.x;
		result.m[3][1] = translate.y;
		result.m[3][2] = translate.z;
		result.m[3][3] = 1.0f;
		return result;
	}

	// スケール行列
	static KamataEngine::Matrix4x4 MakeScale(const KamataEngine::Vector3& scale) {
		KamataEngine::Matrix4x4 result = {};
		result.m[0][0] = scale.x;
		result.m[1][1] = scale.y;
		result.m[2][2] = scale.z;
		result.m[3][3] = 1.0f;
		return result;
	}
