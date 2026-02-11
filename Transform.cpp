#include "Transform.h"

void WorldTransformUpdeta(KamataEngine::WorldTransform& worldTransform) {
		// 4. アフィン変換行列の合成 (Scale * Rotate * Translate)
	worldTransform.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(
		worldTransform.scale_,
		worldTransform.rotation_,
		worldTransform.translation_
	);
	// 5. 行列を定数バッファに転送
	worldTransform.TransferMatrix();
}