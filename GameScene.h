#pragma once
#include "KamataEngine.h"
#include"Player.h"
#include<vector>

class GameScene {

public:
	GameScene();
	~GameScene();

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	//3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* Mapmodel_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*> >worldTransformBlocks_;

	// 初期化関数
	void Initialize();

	// 更新処理関数
	void Updatta();

	// 描画処理
	void Draw();

	Player* player_ = nullptr;

private:
	uint32_t textureHandle_;
	bool isDebugCamera_ = false;
	// プライベートでシーンごとの旋回用の処理関数を追加
};
