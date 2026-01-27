#include "GameScene.h"
//===================================================
//パブリックの処理
//===================================================

using namespace KamataEngine;

//コンストラクタ
//GameScene::GameScene() { 
//	Initialize; 
//}

void GameScene::Initialize() {

	// 初期化処理の追加

	// テクスチャを読み込む

	textureHandle_ = TextureManager::Load("uvChecker.png");

	// スプライトを生成

	sprite_ = Sprite::Create(textureHandle_, {100, 50});
}
void GameScene::Updatta() {

	//更新処理の追加
	Vector2 position = sprite_->GetPosition();
	position.x += 2.0f;
	position.y += 1.0f;

	sprite_->SetPosition(position);
}

void GameScene::Draw() {

	//描画処理の追加

	Sprite::PreDraw();

	sprite_->Draw();
	Sprite::PostDraw();


}


//GameScene::~GameScene() { 
//
//	//解放エクササイズ
//	delete sprite_;
//}
  
//===================================================
//プライベート内の処理
//===================================================


