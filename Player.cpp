#include "Player.h"
#include <cassert>

using namespace KamataEngine;

Player::Player() {}

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	camera_ = camera;
}

void Player::Updata() {

	// 仮のスケール・回転・平行移動値を設定
	KamataEngine::Vector3 scale{1.0f, 1.0f, 1.0f};
	KamataEngine::Vector3 rotation{0.0f, 0.0f, 0.0f};
	KamataEngine::Vector3 translation{0.0f, 0.0f, 0.0f};

	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(scale, rotation, translation);
}

void Player::Draw() { 
	model_->Draw(worldTransform_, *camera_, textureHandle_); 

}

Player::~Player() {}