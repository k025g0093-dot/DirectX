#include "Player.h"
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

	// キーによる移動
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

	// プレイヤーの移動処理
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

#pragma region アフィン行列の作成と行列の更新
	// 仮のスケール・回転・平行移動値を設定
	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_,
	    worldTransform_.translation_ // ここが大事！
	);
	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

#pragma endregion
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

Player::~Player() {}