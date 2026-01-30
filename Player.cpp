#include "Player.h"
#include <cassert>

using namespace KamataEngine;

Player::Player() {

}

void Player::Initialize(KamataEngine::Model* model,
	uint32_t textureHandle, 
	KamataEngine::Camera* camera) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	camera_ = camera;
}


void Player::Updata() {

	worldTransform_.TransferMatrix();

}


void Player::Draw() {
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}


Player::~Player() {


}