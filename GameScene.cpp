#include "GameScene.h"
#include <math\Matrix4x4.h> // 必要に応じてインクルード
#include <cassert>
//===================================================
// パブリックの処理
//===================================================

using namespace KamataEngine;

// コンストラクタ
GameScene::GameScene() { Initialize(); }

// 初期化処理
void GameScene::Initialize() {

	// 初期化処理の追加
	//   トランスフォームとカメラの初期化
	worldTransform_.Initialize();
	camera_.farZ = 2000;
	camera_.Initialize();
	debugCamera_ = new DebugCamera(1280, 720);
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);


	// モデルの生成
	model_ = Model::Create();
	Mapmodel_ = Model::Create();
	modelSkydome_ = Model::Create();
	modelPlayer_ = Model::Create(); 


#pragma region 画像読み込み範囲

	//textureHandle_ = TextureManager::Load("./Resources/cube/cube.jpg");

	//各モデルの読み込み
	modelMap_ = Model::CreateFromOBJ("block", true); //マップのモデルの読み込み
	modelSkydome_ = Model::CreateFromOBJ("skydome", true); // スカイドームのモデルを読み込む
	modelPlayer_ = Model::CreateFromOBJ("player", true);     // ここにモデルを入れる際はモデルなどと同じ名前で
#pragma endregion

	// 引数などの受け渡しの関係上ここから下にplayerとかの初期化関数とかを追加

	// playerや敵などのインスタンスの生成
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_);
	//プレイヤーの初期位置を変えれるようにgetterを追加
	player_->GetWorldTransform().translation_ = {2.0f, 2.0f, 0.0f};//X,Y,Zの順だよ

	//スカイドームの初期化とインスタンスの生成
	SkyDome_ = new SkyDome();
	SkyDome_->Initialize(modelSkydome_);

	//マップチップの初期化とCSVファイルの読み込み
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("./Resources/map.csv");

	//ブロックの生成処理
	GenerateBlocks();
	
	//ブロックのモデルをセッターに入れてそのまま使用できるように
	assert(modelMap_);
	model_ = modelMap_;
}

// 更新処理
void GameScene::Updatta() {

#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_R)) {
		isDebugCamera_ = true;
	}

#endif // DEBUG

	if (isDebugCamera_) {
		// デバックカメラの更新処理
		debugCamera_->Update();

		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}

	// 更新処理の追加
	player_->Updata();
	SkyDome_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {

			if (!worldTransformBlock) {
				continue;
			}

			// 4. アフィン変換行列の合成 (Scale * Rotate * Translate)
			worldTransformBlock->matWorld_ = Matrix4x4::MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			// 5. 行列を定数バッファに転送
			worldTransformBlock->TransferMatrix();
		}
	}
}

// 描画処理
void GameScene::Draw() {
	// 描画処理の追加
	Model::PreDraw();

	//model_->Draw(worldTransform_, camera_, textureHandle_);

	SkyDome_->Draw(&camera_);
	player_->Draw();

	for (std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {

			if (!worldTransformBlock) {
				continue;
			}

			model_->Draw(*worldTransformBlock, camera_);
		}
	}

	Model::PostDraw();
}

// デストラクタ（解放エクササイズ）
GameScene::~GameScene() {
	// モデルの解放
	delete model_;
	delete Mapmodel_;
	delete player_;
	delete debugCamera_;
	delete modelSkydome_;
	delete mapChipField_;
	for (std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();
}
//===================================================
// プライベート内の処理
//===================================================

void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t mumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素の変更
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; i++) {
		worldTransformBlocks_[i].resize(mumBlockHorizontal); // 各列に「行」を確保する
	}

	// キューブ生成
// GameScene.cpp 修正後
for (uint32_t j = 0; j < numBlockVirtical; j++) {
    for (uint32_t i = 0; i < mumBlockHorizontal; i++) {

        // 引数の順番を (i, j) に修正！
        if (mapChipField_->GetMapChipTypeByIndex(i, j) == MapChipType::kBlock) {
            
            WorldTransform* worldTransform = new WorldTransform();
            worldTransform->Initialize();
            
            worldTransformBlocks_[j][i] = worldTransform;
            // ここは (i, j) で合っています
            worldTransformBlocks_[j][i]->translation_ = mapChipField_->GetMapChipPositionByIndex(i, j);
        }
    }
}
}
