#include "GameScene.h"
//===================================================
// パブリックの処理
//===================================================

using namespace KamataEngine;

// コンストラクタ
GameScene::GameScene() { 
	Initialize();

}

void GameScene::Initialize() {

	//初期化処理の追加
	//  トランスフォームとカメラの初期化
	worldTransform_.Initialize();
	camera_.Initialize();
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	// モデルの生成
	model_ = Model::Create();

	// 関数を使っての初期化などあとはnew関連も



#pragma region 画像読み込み範囲

	textureHandle_ = TextureManager::Load("./Resources/uvChecker.png");
#pragma endregion 

	


	//引数などの受け渡しの関係上ここから下にplayerとかの初期化関数とかを追加

	// playerや敵などのインスタンスの生成
	player_ = new Player();
	player_->Initialize(model_, textureHandle_, &camera_);


}

void GameScene::Updatta() {

	//更新処理の追加
	player_->Updata();
}

void GameScene::Draw() {

	//描画処理の追加
	Model::PreDraw();
	player_->Draw();

	//model_->Draw(worldTransform_, camera_, textureHandle_);
	Model::PostDraw();

}

//デストラクタ（解放エクササイズ）
GameScene::~GameScene() {
	// モデルの解放
	delete model_;
	delete player_;

	// 解放エクササイズ
	delete model_;
}
//===================================================
// プライベート内の処理
//===================================================
