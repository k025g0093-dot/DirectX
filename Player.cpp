#include "Player.h"
#include <cassert>

using namespace KamataEngine;

Player::Player() {}

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 180.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	
	assert(model);
	model_ = model;
	//textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	camera_ = camera;
}

void Player::Updata() {

	// 仮のスケール・回転・平行移動値を設定
	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_,
	    worldTransform_.translation_ // ここが大事！
	);
	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}

void Player::Draw() { 
	model_->Draw(worldTransform_, *camera_/*, textureHandle_*/); 

}

Player::~Player() {}