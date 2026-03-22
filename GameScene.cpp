#include "GameScene.h"
#include <cassert>
#include <math\Matrix4x4.h> // 必要に応じてインクルード
//===================================================
// パブリックの処理
//===================================================

using namespace KamataEngine;

// コンストラクタ
GameScene::GameScene() { Initialize(); }

// 初期化処理
void GameScene::Initialize() {

#pragma region 基礎システムの初期化
	worldTransform_.Initialize();
	camera_.farZ = 2000;
	camera_.Initialize();
#pragma endregion

#pragma region インスタンスの生成(new)
	player_ = new Player();
	debugCamera_ = new DebugCamera(1280, 720);
	SkyDome_ = new SkyDome();
	mapChipField_ = new MapChipField();
	cameraController_ = new CameraController();
	deathParticles_ = new DeathParticles();
#pragma endregion

#pragma region モデルの読み込み
	modelMap_ = Model::CreateFromOBJ("block", true);       // マップのモデル
	modelSkydome_ = Model::CreateFromOBJ("skydome", true); // スカイドームのモデル
	modelPlayer_ = Model::CreateFromOBJ("player", true);   // プレイヤーのモデル
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);     // プレイヤーのモデル
	modelParticl_ = Model::CreateFromOBJ("player", true);  // 仮モデルでプレイヤーのモデルを使用
	// 描画用ポインタへの代入
	assert(modelMap_);
	model_ = modelMap_;
#pragma endregion

#pragma region カメラコントローラーの設定
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);

	// ここで距離の微調整が可能
	cameraController_->targetOffset_ = {0.0f, 0.0f, -20.0f};
	cameraController_->SetMovableArea({11, 100, 6, 100}); // カメラの移動できる最大値、最少値
	cameraController_->Reset();

	// ライン描画用カメラをコントローラー側に同期
	PrimitiveDrawer::GetInstance()->SetCamera(&cameraController_->GetCamera());
#pragma endregion

#pragma region マップ・オブジェクトの初期化
	// マップチップ読み込み
	mapChipField_->LoadMapChipCsv("./Resources/map.csv");
	GenerateBlocks();

	// プレイヤー初期化 (座標計算とカメラの紐付け)
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 15);

	for (int32_t i = 0; i < 3; i++) {
		// 1. 毎回新しいメモリを確保する
		Enemy* newEnemy = new Enemy();

		// 2. 座標が重ならないように計算（例：X軸方向に5ずつずらす）
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(17 + i, 14 + i);

		// 3. 新しく作った個体に対して初期化
		newEnemy->Initialize(modelEnemy_, &cameraController_->GetCamera(), enemyPosition);

		// 4. vectorに追加
		enemyis_.push_back(newEnemy);
	}

	// playerの初期化とセッターによる情報の受け渡しw
	player_->Initialize(modelPlayer_, &cameraController_->GetCamera(), playerPosition);
	player_->SetMapChipFiled(mapChipField_); // マップのデータをプレイヤーに渡す

	// やられたときに出るパーティクル（仮として現在プレイヤーのモデルが入っている
	deathParticles_->Initialize(modelParticl_, &cameraController_->GetCamera(), playerPosition);

	// スカイドーム初期化
	SkyDome_->Initialize(modelSkydome_);
#pragma endregion
}

// 更新処理
void GameScene::Updata() {

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

	// 1. プレイヤーの更新（まずプレイヤーが動く）
	player_->Updata();
	for (Enemy* enemy : enemyis_) {

		enemy->Update();
	}
	// 2. カメラコントローラーの更新（動いたプレイヤーをカメラが追いかける）
	// ★これが抜けているので追加してください
	cameraController_->Update();

	SkyDome_->Update();

	if (Input::GetInstance()->TriggerKey(DIK_E)) {
		deathParticles_->isFinished_ = false;
		deathParticles_->Initialize(modelParticl_, &cameraController_->GetCamera(), player_->GetWorldPodition());

	}

	//パーティクルの処理
	if (!deathParticles_->isFinished_) {
		deathParticles_->Updata();
	}

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

	CheckAllCollisions();
}

// 描画処理
void GameScene::Draw() {

	// コントローラー側のカメラを取得
	Camera& activeCamera = cameraController_->GetCamera();

	// 描画の開始位置
	Model::PreDraw();

#pragma region 背景描画

	SkyDome_->Draw(&activeCamera);
#pragma endregion

#pragma region キャラクター描画
	player_->Draw(); // 内部で保持しているカメラを使用
	for (Enemy* enemy : enemyis_) {

		enemy->Draw();
	}

	if (!deathParticles_->isFinished_) {
		deathParticles_->Draw();
	}

#pragma endregion

#pragma region マップ描画
	for (std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			if (worldTransformBlock) {
				model_->Draw(*worldTransformBlock, activeCamera);
			}
		}
	}
#pragma endregion

	// 描画の終了位置
	Model::PostDraw();
}

// デストラクタ（解放エクササイズ）
GameScene::~GameScene() {
	// モデルの解放
	delete model_;
	delete Mapmodel_;
	delete player_;
	delete enemy_;
	delete debugCamera_;
	delete modelSkydome_;
	delete mapChipField_;
	delete deathParticles_;
	for (std::vector<WorldTransform*>& worldTransformBlockRow : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockRow) {
			delete worldTransformBlock;
		}
	}
	for (Enemy* enemy : enemyis_) {
		delete enemy;
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

void GameScene::CheckAllCollisions() {

	AABB aabb1, aabb2;

	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemyis_) {

		aabb2 = enemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {
			player_->OnCollsion(enemy);
			enemy->OnCollsion(player_);
		}
	}
}
