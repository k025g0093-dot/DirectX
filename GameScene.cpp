#include "GameScene.h"
#include <math\Matrix4x4.h> // 必要に応じてインクルード
//===================================================
// パブリックの処理
//===================================================

using namespace KamataEngine;

// コンストラクタ
GameScene::GameScene() { Initialize(); }


//初期化処理
void GameScene::Initialize() {

	// 初期化処理の追加
	//   トランスフォームとカメラの初期化
	worldTransform_.Initialize();
	camera_.farZ = 2000;
	camera_.Initialize();
	debugCamera_ = new DebugCamera(1280,720);
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	// モデルの生成
	model_ = Model::Create();
	Mapmodel_ = Model::Create();
	modelSkydome_ = Model::Create();
	modelPlayer_ = Model::Create(); // これだと中身が空
	// 関数を使っての初期化などあとはnew関連も

	// マップのブロックとかの初期化


	// 要素数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;

	// ブロック一個分の幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	// 要素の変更
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal); // 各列に「行」を確保する
	}

	// キューブ生成
	for (uint32_t j = 0; j < kNumBlockVirtical; j++) {
		for (uint32_t i = 0; i < kNumBlockHorizontal; i++) {

			// マップに穴を開けられるように
			if (i >= 0 && i <= 10 && j <= 2) {
				worldTransformBlocks_[j][i] = nullptr; // nullptrを入れておく
				continue;                              // 次のループへ（newを飛ばす）
			}


			// 配列の添字を [j][i] に変更
			worldTransformBlocks_[j][i] = new WorldTransform();
			worldTransformBlocks_[j][i]->Initialize();

			// x座標には横方向の変数 i を使う
			worldTransformBlocks_[j][i]->translation_.x = kBlockWidth * (float)i;
			// y座標には縦方向の変数 j を使う（ここを i にすると斜めになるので j に修正）
			worldTransformBlocks_[j][i]->translation_.y = kBlockHeight * (float)j;
			worldTransformBlocks_[j][i]->translation_.z = 0.0f;
			// 行列の更新を忘れずに
			worldTransformBlocks_[j][i]->TransferMatrix();
		}
	}

#pragma region 画像読み込み範囲

	textureHandle_ = TextureManager::Load("./Resources/cube/cube.jpg");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);//スカイドームのモデルを読み込む
	modelPlayer_ = Model::CreateFromOBJ("Yeti", true);//ここにモデルを入れる際はモデルなどと同じ名前で
#pragma endregion

	// 引数などの受け渡しの関係上ここから下にplayerとかの初期化関数とかを追加


	// playerや敵などのインスタンスの生成
	player_ = new Player();
	player_->Initialize(modelPlayer_/*, textureHandle_*/, &camera_);

	SkyDome_ = new SkyDome();
	SkyDome_->Initialize(modelSkydome_);
}


//更新処理
void GameScene::Updatta() {


#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_R)) {
		isDebugCamera_ = true;
	}

#endif // DEBUG

	if (isDebugCamera_) {
		//デバックカメラの更新処理
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

//描画処理
void GameScene::Draw() {
	// 描画処理の追加
	Model::PreDraw();

	model_->Draw(worldTransform_, camera_, textureHandle_);

	SkyDome_->Draw(&camera_);
	player_->Draw();


	for (std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {

			if (!worldTransformBlock) {
				continue;
			}

			Mapmodel_->Draw(*worldTransformBlock, camera_, textureHandle_);
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
