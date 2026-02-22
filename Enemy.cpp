#define NOMINMAX
#include "Enemy.h"
#include <algorithm>
#include <cassert>
#include <numbers>

using namespace KamataEngine;

//初期化処理
void Enemy::Initialize(
    KamataEngine::Model* model, 
    KamataEngine::Camera* camera, 
    const KamataEngine::Vector3& position
) {

    	// アフィン行列に必要な者たち
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 180.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	velocity_ = {0.0f, 0.0f, 0.0f}; // 加速度の初期値

	assert(model); // セッターにモデルを入れる
	model_ = model;

	velocity_ = {-kWalkSpeed, 0, 0};

	// textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	camera_ = camera;

	//にメーション処理の初期化
	walkTimer_ = 0.0f;
}

//更新処理
void Enemy::Update() {

	walkTimer_ += 1.0f / 60.0f;
	float param= std::sin((std::numbers::pi_v<float> * 2.0f) * walkTimer_);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = degree;
	worldTransform_.translation_ += velocity_;

#pragma region アフィン行列の作成と行列の更新
	// 仮のスケール・回転・平行移動値を設定
	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

#pragma endregion
}


void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }
