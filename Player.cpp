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
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2;
	camera_ = camera;
}

void Player::Updata() {

#pragma region プレイヤーの移動処理

	MovePlayer();

	// 衝突情報の初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の価をコピー
	collisionMapInfo.velocity_ = velocity_;

	// マップの衝突判定
	MapCollsion(collisionMapInfo);
	CheckedMove(collisionMapInfo);

	IsHitTop(collisionMapInfo);

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

	// 着地フラグ
	bool landing = false;
	if (velocity_.y < 0) {

		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

#pragma region プレイヤーの移動処理
	if (onGround_) {

		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 別の専用の加速をついか
			Vector3 acceleration = {};

			// 移動処理
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// プレイヤーの画像の向きの変更
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;

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
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

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

	// 接着判定
	if (onGround_) {

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に以降
			onGround_ = false;
		}
	} else {
		// 着地判定
		if (landing) {

			// めり込み防止
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向減衰
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接着に移行
			onGround_ = true;
		}
	}
}

void Player::CheckedMove(const CollisionMapInfo& info) { worldTransform_.translation_ += info.velocity_; }

#pragma region playerとマップチップの当たり判定

void Player::MapCollsion(CollisionMapInfo& info) {
	// 1. 水平方向（左右）の判定
	if (velocity_.x > 0) {
		CheckCollisionRight(info);
	} else if (velocity_.x < 0) {
		CheckCollisionLeft(info);
	}

	// 2. 垂直方向（上下）の判定
	if (velocity_.y > 0) {
		CheckCollisionTop(info);
	} else {
		// 落下中、または接地中の維持判定
		CheckCollisionBottom(info);
	}
}

void Player::CheckCollisionRight(CollisionMapInfo& info) {
	// ここに右側の判定を記述する
	(void)info;
}

void Player::CheckCollisionLeft(CollisionMapInfo& info) {
	// ここに左側の判定を記述する
	(void)info;
}

void Player::CheckCollisionTop(CollisionMapInfo& info) {
	// ここに上側の判定を記述する

	if (info.velocity_.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPositio(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
	}

	MapChipType maoChipType;
	// 真上の判定
	bool hit = false;
	// 左上の点の当たり判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	maoChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (maoChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	maoChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (maoChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込み排除
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_+velocity_);
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.velocity_.y = std::max(0.0f, (rect.bottom- (kHight / 2) + 0.01f));
		// 天井に当たったことを記録
		info.isFloor = true;
	}
}

#pragma endregion

//天井に当たったときに速度を０にする処理
void Player::IsHitTop(const CollisionMapInfo& info) {

	if (info.isFloor) {
		DebugText::GetInstance()->ConsolePrintf("hit ceilong\n");

		velocity_.y = 0;
	}
}

void Player::CheckCollisionBottom(CollisionMapInfo& info) {
	// ここに下側の判定を記述する
	(void)info;
}

Vector3 Player::CornerPositio(const Vector3& center, Corner corner) {
	float halfW = kWidth / 2.0f;
	float halfH = kHight / 2.0f;

	Vector3 offsetTable[kNumCorner] = {
	    {halfW,  -halfH, 0.0f}, // kRightBottom
	    {-halfW, -halfH, 0.0f}, // kLeftBottom
	    {halfW,  halfH,  0.0f}, // kRightTop
	    {-halfW, halfH,  0.0f}  // kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}
