#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "SkyDome.h"
#include "CameraController.h"
#include <vector>

class GameScene {

public:
	GameScene();
	~GameScene();

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

#pragma region	3Dに非一ようなものを呼び出すもの

	// 3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* Mapmodel_ = nullptr;     // マップのモデル
	KamataEngine::Model* modelSkydome_ = nullptr; // スカイドームのモデル
	KamataEngine::Model* modelPlayer_ = nullptr;  // プレイヤーのモデル
	KamataEngine::Model* modelMap_ = nullptr;     // プレイヤーのモデル

#pragma endregion

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// 初期化関数
	void Initialize();

	// 更新処理関数
	void Updata();

	// 描画処理
	void Draw();

	void GenerateBlocks();

	Player* player_ = nullptr;
	SkyDome* SkyDome_ = nullptr;
	MapChipField* mapChipField_;
	CameraController* cameraController_;

private:
	uint32_t textureHandle_;
	bool isDebugCamera_ = false;
	// プライベートでシーンごとの旋回用の処理関数を追加
};
