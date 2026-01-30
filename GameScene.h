#pragma once
#include "KamataEngine.h"
#include"Player.h"

class GameScene {

public:
	GameScene();
	~GameScene();

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;

	//3Dモデルで必要なモデルの呼び出し
	KamataEngine::Model* model_ = nullptr;


	// 初期化関数
	void Initialize();

	// 更新処理関数
	void Updatta();

	// 描画処理
	void Draw();

	Player* player_ = nullptr;

private:
	uint32_t textureHandle_;

	// プライベートでシーンごとの旋回用の処理関数を追加
};
