#include "TitleScene.h"
#include <cassert>

using namespace KamataEngine;

void TitleScene::Initialize() {
	modelTitle_ = Model::CreateFromOBJ("player", true);
	assert(modelTitle_);

	worldTransform_.Initialize();
	camera_.Initialize();
	objectColor_.Initialize();

	// 1. キャラを正面に向ける設定
	worldTransform_.scale_ = {1.5f, 1.5f, 1.5f};    // 少し大きく設定
	worldTransform_.rotation_ = {0.0f, 180.0f, 0.0f}; // 向きが逆なら 3.1415f (180度) に
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	// 2. カメラの初期位置（キャラの正面に配置）
	// zをマイナスにすると手前になります
	camera_.translation_ = {0.0f, 0.5f, -5.0f};
}

void TitleScene::Updata() {

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}

	// --- キャラの行列更新 ---
	worldTransform_.rotation_.y -= 0.1f;
	worldTransform_.matWorld_ = KamataEngine::Matrix4x4::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	// --- カメラの行列更新 ---
	// 座標などを変えた後は、UpdateMatrix を呼ばないと画面に反映されません
	camera_.UpdateMatrix();
}

void TitleScene::Draw() {
	// ★GameScene.cpp の 160行目〜180行目付近と全く同じ書き方です
	// これがないと commandList が nullptr になります

	// 1. 3Dモデル描画前の準備（これが必要）
	Model::PreDraw();

	// 2. 描画（引数は 3つ）
	if (modelTitle_) {
		modelTitle_->Draw(worldTransform_, camera_, &objectColor_);
	}

	// 3. 3Dモデル描画後の終了処理
	Model::PostDraw();
}