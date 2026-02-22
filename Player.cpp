#define NOMINMAX
#include "Player.h"
#include "MapChipField.h" // ← 追加：型定義を完全にするため
#include <algorithm>
#include <cassert>
#include <numbers>

using namespace KamataEngine;

Player::Player() {}

void Player::Initialize(
    KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position

) {

	// アフィン行列に必要な者たち
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 180.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	velocity_ = {0.0f, 0.0f, 0.0f}; // 加速度の初期値

	assert(model); // セッターにモデルを入れる
	model_ = model;

	// textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	camera_ = camera;


}

void Player::Updata() {

	// onGround_ = false;
#pragma region プレイヤーの移動処理

	MovePlayer();

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.velocity_ = velocity_;
	IsHitWall(collisionMapInfo);
	// 1. 衝突判定と押し戻し量の計算
	MapCollsion(collisionMapInfo);

	// 2. 状態判定（座標を動かす前に、計算された移動量を使って判定を済ませる）
	IsHitTop(collisionMapInfo);
	IsGrounded(collisionMapInfo);

	// 3. 最後に一回だけ座標を確定させる
	CheckedMove(collisionMapInfo);
#pragma endregion

#pragma region プレイヤーの旋回処理

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 20.0f;
		if (turnTimer_ < 0.0f) {

			turnTimer_ = 0.0f;
		}
		// 進捗率 (0.0=開始, 1.0=終了)
		float t = 1.0f - turnTimer_;

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 計算式
		worldTransform_.rotation_.y = turnFistRotationY_ + (destinationRotationY - turnFistRotationY_) * t;
	}

#pragma endregion

	// プレイヤーの移動処理
	// worldTransform_.translation_ += velocity_;
#pragma region アフィン行列の作成と行列の更新
	// 仮のスケール・回転・平行移動値を設定
	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

#pragma endregion
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

Player::~Player() {}

void Player::MovePlayer() {

#pragma region プレイヤーの移動処理
	if (onGround_) {

		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 別の専用の加速をついか
			Vector3 acceleration = {};

			// 移動処理
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// プレイヤーの画像の向きの変更
				if (lrDirection_ != LRDirection::kRight) { // 今が右じゃないなら
					lrDirection_ = LRDirection::kRight;

					// 旋回時のアニメーションのためのもの
					turnFistRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 1.0f;
				}

				// 逆方向時のブレーキ処理
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x += kAcceleration;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				// プレイヤーの画像の向きの変更
				if (lrDirection_ != LRDirection::kLeft) { // 今が左じゃないなら
					lrDirection_ = LRDirection::kLeft;

					// 旋回時のアニメーションのためのもの
					turnFistRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 1.0f;
				}

				// 逆方向時のブレーキ処理
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;
			}
			// 速度の加算
			velocity_.x = acceleration.x;

			// 最大速度の制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 減衰処理
			velocity_.x *= (1.0f - kAttenuation);
		}

		// ジャンプの処理
		if (Input::GetInstance()->PushKey(DIK_UP)) {

			velocity_ += Vector3(0, kJumpAcceleration, 0);
		}

#pragma endregion

	} else {

		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::CheckedMove(const CollisionMapInfo& info) { worldTransform_.translation_ += info.velocity_; }

#pragma region playerとマップチップの当たり判定

void Player::MapCollsion(CollisionMapInfo& info) {
	// 1. まず垂直方向（上下）を先にやる
	CheckCollisionTop(info);
	CheckCollisionBottom(info);

	// 2. そのあとに水平方向（左右）をやる
	CheckCollisionRight(info);
	CheckCollisionLeft(info);
}
#pragma endregion

#pragma region 横方向の当たり判定

void Player::CheckCollisionRight(CollisionMapInfo& info) {
	if (info.velocity_.x > 0.0f) {
		std::array<Vector3, kNumCorner> positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
		}

		MapChipField::IndexSet indexSet;
		bool hit = false;
		MapChipField::IndexSet hitIndex{};

		// 右上の判定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop] + Vector3(0.1f, 0, 0));
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		// 右下の判定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0.1f, 0.5f, 0));
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		if (hit) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(hitIndex.xIndex, hitIndex.yIndex);
			// 右移動なので、キャラの右端をブロックの左端に合わせる
			float newVelocityX = (rect.left - worldTransform_.translation_.x) - (kWidth / 2.0f) - 0.01f;
			info.velocity_.x = std::min(0.0f, newVelocityX);
			info.isWall = true;
		}
	}
}

void Player::CheckCollisionLeft(CollisionMapInfo& info) {
	if (info.velocity_.x < 0.0f) {
		std::array<Vector3, kNumCorner> positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
		}

		MapChipField::IndexSet indexSet;
		bool hit = false;
		MapChipField::IndexSet hitIndex{};

		// 左上の判定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop] + Vector3(-0.1f, 0, 0));
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		// 左下の判定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(-0.1f, 0.5f, 0));
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		if (hit) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(hitIndex.xIndex, hitIndex.yIndex);
			// 左移動なので、キャラの左端をブロックの右端に合わせる
			float newVelocityX = (rect.right - worldTransform_.translation_.x) + (kWidth / 2.0f) + 0.01f;
			info.velocity_.x = std::max(0.0f, newVelocityX);
			info.isWall = true;
		}
	}
}

void Player::IsHitWall(const CollisionMapInfo& info) {

	if (info.isWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

#pragma endregion

#pragma region playerとマップチップの上方向判定

void Player::CheckCollisionTop(CollisionMapInfo& info) {
	if (info.velocity_.y > 0.0f) {
		std::array<Vector3, kNumCorner> positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
		}

		MapChipField::IndexSet indexSet;
		bool hit = false;
		MapChipField::IndexSet hitIndex{};

		// 左上の判定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		// 右上の判定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
			hitIndex = indexSet;
		}

		if (hit) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(hitIndex.xIndex, hitIndex.yIndex);
			float newVelocityY = (rect.bottom - worldTransform_.translation_.y) - (kHight / 2.0f) - 0.01f;
			info.velocity_.y = std::min(0.0f, newVelocityY);
			info.isFloor = true;
		}
	}
}

// 天井に当たったときに速度を０にする処理
void Player::IsHitTop(const CollisionMapInfo& info) {

	if (info.isFloor) {
		DebugText::GetInstance()->ConsolePrintf("hit ceilong\n");

		velocity_.y = 0;
	}
}

#pragma endregion

#pragma region playerとマップチップの地面判定

void Player::CheckCollisionBottom(CollisionMapInfo& info) {
	// 1. 上昇中は判定しない
	if (info.velocity_.y > 0.0f)
		return;

	// 2. 移動後の四隅の座標を計算 (スライドの最初の工程)
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
	}

	MapChipField::IndexSet indexSet; // 資料にある変数
	bool hit = false;

	// 3. 左下の判定 (スライドの左側の処理)
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		hit = true;
	}
	// 4. 右下の判定 (スライドの右側の処理)
	else {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
			hit = true;
		}
	}

	// 5. 当たっていたら「資料の数式」をそのまま実行
	if (hit) {
		// ※ ここで indexSet は「当たった方の足」の情報のまま残っている
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		// スライドの図にある「青矢印」と「緑矢印」の計算
		// Y移動量 = (ブロック上端 - 移動前座標) + キャラの半径
		info.velocity_.y = (rect.top - worldTransform_.translation_.y) + (kHight / 2.0f) + 0.01f;

		info.isRoof = true; // 接地通知
	}
}

void Player::IsGrounded(const CollisionMapInfo& info) {

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {

			std::array<Vector3, kNumCorner> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
			}

			MapChipType maoChipType;
			// 真上の判定
			bool hit = false;
			// 左下の点の当たり判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -0.05f, 0));
			maoChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (maoChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下の当たり判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -0.05f, 0));
			maoChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (maoChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				onGround_ = false;
			}
		}

	} else {

		if (info.isRoof) {
			DebugText::GetInstance()->ConsolePrintf("isRoof ceilong\n");
			// 着地状態の切り替え
			onGround_ = true;
			// 接着時にX速度の減速
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度を0にする
			velocity_.y = 0.0f;
		}
	}
}

#pragma endregion

Vector3 Player::CornerPositio(const Vector3& center, Corner corner) {
	float halfW = kWidth / 2.0f;
	float halfH = kHight / 2.0f;

	Vector3 offsetTable[kNumCorner] = {
	    {+halfW, -halfH, 0.0f}, // kRightBottom
	    {-halfW, -halfH, 0.0f}, // kLeftBottom
	    {+halfW, +halfH, 0.0f}, // kRightTop
	    {-halfW, +halfH, 0.0f}  // kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}
